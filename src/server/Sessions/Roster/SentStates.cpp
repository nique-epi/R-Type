#include "SentStates.hpp"
#include <algorithm>
#include <cstdint>
#include <utility>
#include "GuaranteedDelivery.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "SentState.hpp"
#include "SentStatesConstants.hpp"

namespace rtype::server {

namespace {

bool everyPartAcknowledged(const network::PacketHeader& header,
                           const SentState& state) {
  return !state.sequences.empty() &&
         std::ranges::all_of(state.sequences,
                             [&header](std::uint16_t sequence) {
                               return network::acknowledges(header, sequence);
                             });
}

}  // namespace

void SentStates::remember(SentState state) {
  if (!history_.empty() && state.tick <= history_.back().tick) {
    history_.clear();
    acknowledgedTick_.reset();
  }
  history_.push_back(std::move(state));
  if (history_.size() > SENT_STATE_HISTORY_SIZE) {
    history_.pop_front();
  }
}

void SentStates::acknowledge(const network::PacketHeader& header) {
  for (auto state = history_.rbegin(); state != history_.rend(); ++state) {
    if (acknowledgedTick_.has_value() && state->tick <= *acknowledgedTick_) {
      return;
    }
    if (everyPartAcknowledged(header, *state)) {
      acknowledgedTick_ = state->tick;
      return;
    }
  }
}

const SentState* SentStates::referenceFor(std::uint32_t tick) const {
  if (!acknowledgedTick_.has_value() || tick <= *acknowledgedTick_ ||
      tick - *acknowledgedTick_ > network::MAXIMUM_REFERENCE_DISTANCE) {
    return nullptr;
  }
  const auto found =
      std::ranges::find(history_, *acknowledgedTick_, &SentState::tick);
  return found == history_.end() ? nullptr : &*found;
}

}  // namespace rtype::server
