#include "WorldStateSender.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "DeltaWorldState.hpp"
#include "DeltaWorldStateCodec.hpp"
#include "Entity.hpp"
#include "EntitySpawnSender.hpp"
#include "EntityTypeNumber.hpp"
#include "Intent.hpp"
#include "Invulnerable.hpp"
#include "Lives.hpp"
#include "ManeuverKind.hpp"
#include "NetworkException.hpp"
#include "NetworkIdentity.hpp"
#include "PlayerRoster.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ReplicationConstants.hpp"
#include "SentState.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "SessionDatagram.hpp"
#include "ShipDirections.hpp"
#include "ShipMotion.hpp"
#include "StateDelta.hpp"
#include "StateEncoding.hpp"
#include "StateSending.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateDelta.hpp"

namespace rtype::server {

namespace {

static_assert(game::PLAYFIELD_WIDTH <= LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE &&
                  game::PLAYFIELD_HEIGHT <= LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE,
              "a larger playfield no longer fits a wire coordinate");

/** @returns A byte with only the bit at @p position set. */
std::uint8_t bitAt(std::uint8_t position) {
  return static_cast<std::uint8_t>(1U << static_cast<unsigned int>(position));
}

/** @returns The bits of @p directions, where a PlayerState carries them in its
 * motion. */
std::uint8_t directionBitsOf(const game::ShipDirections& directions) {
  std::uint8_t bits = 0;
  if (directions.up) {
    bits |= bitAt(network::UP_ACTION_BIT_POSITION);
  }
  if (directions.down) {
    bits |= bitAt(network::DOWN_ACTION_BIT_POSITION);
  }
  if (directions.left) {
    bits |= bitAt(network::LEFT_ACTION_BIT_POSITION);
  }
  if (directions.right) {
    bits |= bitAt(network::RIGHT_ACTION_BIT_POSITION);
  }
  return bits;
}

/** @returns The bit of the motion that tells @p kind; 0 for no maneuver. */
std::uint8_t maneuverBitOf(game::ManeuverKind kind) {
  switch (kind) {
    case game::ManeuverKind::Roll:
      return bitAt(network::ROLLING_MOTION_BIT_POSITION);
    case game::ManeuverKind::Dash:
      return bitAt(network::DASHING_MOTION_BIT_POSITION);
    case game::ManeuverKind::None:
      return 0;
  }
  return 0;
}

/** @brief What a PlayerState reads off the ship of a player. */
struct ShipReading {
  std::optional<std::uint8_t> lives;
  bool invulnerable{false};
  std::uint8_t motion{0};
};

/** @returns What the ship whose network id is @p shipId in @p components tells
 * a PlayerState: its lives and protection, and the bits of the directions its
 * Intent holds and of its maneuver. Nothing is read when no ship has that id
 * and a ShipMotion. */
ShipReading readShip(engine::ComponentRegistry& components,
                     std::uint32_t shipId) {
  ShipReading reading;
  components.forEach<game::ShipMotion, game::NetworkIdentity>(
      [&components, shipId, &reading](engine::Entity ship,
                                      const game::ShipMotion& motion,
                                      const game::NetworkIdentity& identity) {
        if (identity.networkId != shipId) {
          return;
        }
        reading.motion = maneuverBitOf(motion.maneuver);
        if (const game::Intent* const intent =
                components.get<game::Intent>(ship)) {
          reading.motion |=
              directionBitsOf(intent->steered ? motion.heldAtLastInput
                                              : game::ShipDirections{});
        }
        if (const game::Lives* const lives =
                components.get<game::Lives>(ship)) {
          reading.lives = lives->count;
        }
        if (const game::Invulnerable* const protection =
                components.get<game::Invulnerable>(ship)) {
          reading.invulnerable = protection->ticksLeft > 0;
        }
      });
  return reading;
}

/** @returns @p state written as the difference from @p reference, or in full
 * when there is none. */
network::DeltaWorldState deltaAgainst(const network::WorldState& state,
                                      const SentState* reference) {
  const std::span<const network::EntityState> base =
      reference == nullptr
          ? std::span<const network::EntityState>{}
          : std::span<const network::EntityState>{*reference->entities};
  network::StateDelta difference = network::diffStates(base, state.entities);
  const auto baseDistance =
      reference == nullptr
          ? network::NO_REFERENCE_DISTANCE
          : static_cast<std::uint8_t>(state.tick - reference->tick);
  return network::DeltaWorldState{.tick = state.tick,
                                  .baseDistance = baseDistance,
                                  .partIndex = 0,
                                  .partCount = 1,
                                  .players = state.players,
                                  .changed = std::move(difference.changed),
                                  .removed = std::move(difference.removed)};
}

}  // namespace

WorldStateSender::WorldStateSender(PlayerRoster& roster, SendDatagram send,
                                   StateSending sending)
    : roster_(&roster),
      send_(std::move(send)),
      sending_(sending),
      logger_(std::string{REPLICATION_LOGGER_NAME}) {
  if (sending.statesPerSecond < LOWEST_WORLD_STATES_PER_SECOND ||
      sending.statesPerSecond > HIGHEST_WORLD_STATES_PER_SECOND) {
    throw InvalidStateRateException(sending.statesPerSecond);
  }
}

bool WorldStateSender::carriesState(std::uint32_t tick) const {
  const std::uint64_t shareOfSecond = static_cast<std::uint64_t>(tick) *
                                      sending_.statesPerSecond %
                                      engine::SIMULATION_TICKS_PER_SECOND;
  return shareOfSecond < sending_.statesPerSecond;
}

void WorldStateSender::onTick(std::uint32_t tick,
                              engine::ComponentRegistry& components) {
  if (!carriesState(tick)) {
    return;
  }
  const ConnectedPlayers players = roster_->connected();
  if (players.empty()) {
    return;
  }
  std::size_t leftOutCount = 0;
  network::WorldState state = capture(tick, components, leftOutCount);
  state.players = playerStatesOf(players, components);
  reportLeftOut(leftOutCount);
  std::ranges::sort(state.entities, {}, &network::EntityState::entityId);
  const auto entities =
      std::make_shared<const std::vector<network::EntityState>>(state.entities);
  for (ConnectedPlayer* const player : players) {
    sendTo(*player, state, entities);
  }
}

void WorldStateSender::sendTo(
    ConnectedPlayer& player, const network::WorldState& state,
    std::shared_ptr<const std::vector<network::EntityState>> entities) {
  const SentState* const reference =
      sending_.encoding == StateEncoding::Delta
          ? player.sentStates.referenceFor(state.tick)
          : nullptr;
  const network::DeltaWorldState delta = deltaAgainst(state, reference);
  const auto payloads = encodedPartsOf(delta);
  if (!payloads.has_value()) {
    return;
  }
  std::vector<std::uint16_t> sequences;
  sequences.reserve(payloads->size());
  for (const std::vector<std::byte>& payload : *payloads) {
    sequences.push_back(player.nextSequence);
    send_(sessionDatagram(player, network::DeltaWorldState::TYPE, payload));
  }
  player.sentStates.remember(SentState{.tick = state.tick,
                                       .sequences = std::move(sequences),
                                       .entities = std::move(entities)});
}

std::optional<std::vector<std::vector<std::byte>>>
WorldStateSender::encodedPartsOf(const network::DeltaWorldState& delta) {
  try {
    return network::DeltaWorldStateCodec::encode(delta);
  } catch (const network::TooManyDeltaPartsException& tooMany) {
    logger_.error(tooMany.what());
    return std::nullopt;
  }
}

std::vector<network::PlayerState> WorldStateSender::playerStatesOf(
    const ConnectedPlayers& players, engine::ComponentRegistry& components) {
  std::vector<network::PlayerState> states;
  states.reserve(players.size());
  for (const ConnectedPlayer* const player : players) {
    const ShipReading ship = readShip(components, player->shipEntityId);
    states.push_back(network::PlayerState{
        .playerNumber = player->playerNumber,
        .shipEntityId = player->shipEntityId,
        .lives = ship.lives.value_or(player->livesWithoutShip),
        .score = player->score,
        .invulnerable = ship.invulnerable,
        .motion = ship.motion,
        .acknowledgedInput = player->appliedInputNumber.value_or(0)});
  }
  return states;
}

network::WorldState WorldStateSender::capture(
    std::uint32_t tick, engine::ComponentRegistry& components,
    std::size_t& leftOutCount) {
  network::WorldState state{.tick = tick, .players = {}, .entities = {}};
  components.forEach<game::NetworkIdentity, game::Position>(
      [&components, &state, &leftOutCount](
          engine::Entity entity, const game::NetworkIdentity& identity,
          const game::Position& position) {
        const game::EntityTypeNumber* type =
            components.get<game::EntityTypeNumber>(entity);
        if (type == nullptr) {
          return;
        }
        if (!std::isfinite(position.x) || !std::isfinite(position.y)) {
          ++leftOutCount;
          return;
        }
        state.entities.push_back(
            network::EntityState{.entityId = identity.networkId,
                                 .entityType = type->number,
                                 .x = position.x,
                                 .y = position.y});
      });
  return state;
}

void WorldStateSender::reportLeftOut(std::size_t leftOutCount) {
  if (leftOutCount != reportedLeftOutCount_ && leftOutCount != 0) {
    logger_.warn(leftOutCount,
                 " entities left out of the world state: their position is "
                 "not a finite number");
  }
  reportedLeftOutCount_ = leftOutCount;
}

}  // namespace rtype::server
