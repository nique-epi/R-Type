#include "RecordingPlayerInputLink.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include "ActionSet.hpp"
#include "ProtocolConstants.hpp"

std::optional<std::uint32_t> RecordingPlayerInputLink::sendPlayerInput(
    rtype::client::ActionSet held) {
  const auto number = static_cast<std::uint32_t>(
      rtype::network::FIRST_INPUT_NUMBER + sentActions_.size());
  sentActions_.push_back(held.bits());
  return number;
}

const std::vector<std::uint8_t>& RecordingPlayerInputLink::sentActions() const {
  return sentActions_;
}
