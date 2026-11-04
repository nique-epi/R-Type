#include "WorldStateAssembler.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "DeltaWorldState.hpp"
#include "EntityMessageConstants.hpp"
#include "PendingDelta.hpp"
#include "ProtocolConstants.hpp"
#include "StateDelta.hpp"
#include "WorldState.hpp"
#include "WorldStateDelta.hpp"

namespace rtype::client {

namespace {

/** @returns The changed entities and the removed ids of the parts of
 * @p pending, in the order of the parts. */
network::StateDelta joinedDelta(const PendingDelta& pending) {
  network::StateDelta joined;
  for (const std::optional<network::DeltaWorldState>& part : pending.parts) {
    if (part.has_value()) {
      joined.changed.insert(joined.changed.end(), part->changed.begin(),
                            part->changed.end());
      joined.removed.insert(joined.removed.end(), part->removed.begin(),
                            part->removed.end());
    }
  }
  return joined;
}

/** @returns The players of the first part of @p pending that arrived: every
 * part lists them all. */
std::vector<network::PlayerState> playersOf(const PendingDelta& pending) {
  for (const std::optional<network::DeltaWorldState>& part : pending.parts) {
    if (part.has_value()) {
      return part->players;
    }
  }
  return {};
}

}  // namespace

std::optional<network::WorldState> WorldStateAssembler::add(
    const network::DeltaWorldState& part) {
  const bool sameTick = pending_.has_value() && part.tick == pending_->tick;
  if (!sameTick) {
    if (goesBack(part)) {
      clear();
    }
    pending_ = PendingDelta{
        .tick = part.tick,
        .baseDistance = part.baseDistance,
        .parts = std::vector<std::optional<network::DeltaWorldState>>(
            part.partCount),
        .received = 0};
  }
  if (!fillsASlot(part)) {
    return std::nullopt;
  }
  pending_->parts.at(part.partIndex) = part;
  ++pending_->received;
  if (pending_->received < pending_->parts.size()) {
    return std::nullopt;
  }
  return rebuild();
}

void WorldStateAssembler::clear() {
  pending_.reset();
  history_.clear();
}

bool WorldStateAssembler::goesBack(const network::DeltaWorldState& part) const {
  const bool beforePending = pending_.has_value() && part.tick < pending_->tick;
  const bool beforeRebuilt =
      !history_.empty() && part.tick <= history_.back().tick;
  return beforePending || beforeRebuilt;
}

bool WorldStateAssembler::fillsASlot(
    const network::DeltaWorldState& part) const {
  return pending_.has_value() && part.partCount == pending_->parts.size() &&
         part.baseDistance == pending_->baseDistance &&
         part.partIndex < pending_->parts.size() &&
         !pending_->parts.at(part.partIndex).has_value();
}

std::optional<network::WorldState> WorldStateAssembler::rebuild() {
  if (!pending_.has_value()) {
    return std::nullopt;
  }
  const PendingDelta pending = std::move(*pending_);
  pending_.reset();
  const network::WorldState* const reference = referenceOf(pending);
  if (pending.baseDistance != network::NO_REFERENCE_DISTANCE &&
      reference == nullptr) {
    logger_.warn("world state of tick ", pending.tick,
                 " is written against a state this client does not hold, "
                 "ticks back: ",
                 static_cast<unsigned int>(pending.baseDistance));
    return std::nullopt;
  }
  const std::span<const network::EntityState> base =
      reference == nullptr
          ? std::span<const network::EntityState>{}
          : std::span<const network::EntityState>{reference->entities};
  network::WorldState state{
      .tick = pending.tick,
      .players = playersOf(pending),
      .entities = network::applyDelta(base, joinedDelta(pending))};
  remember(state);
  return state;
}

const network::WorldState* WorldStateAssembler::referenceOf(
    const PendingDelta& pending) const {
  if (pending.baseDistance == network::NO_REFERENCE_DISTANCE ||
      pending.tick < pending.baseDistance) {
    return nullptr;
  }
  const std::uint32_t referenceTick = pending.tick - pending.baseDistance;
  const auto found =
      std::ranges::find(history_, referenceTick, &network::WorldState::tick);
  return found == history_.end() ? nullptr : &*found;
}

void WorldStateAssembler::remember(const network::WorldState& state) {
  history_.push_back(state);
  if (history_.size() > REBUILT_STATE_HISTORY_SIZE) {
    history_.pop_front();
  }
}

}  // namespace rtype::client
