#include "RecordingPlayerInputLink.hpp"
#include <cstdint>
#include <vector>
#include "ActionSet.hpp"

void RecordingPlayerInputLink::sendPlayerInput(rtype::client::ActionSet held) {
  sentActions_.push_back(held.bits());
}

const std::vector<std::uint8_t>& RecordingPlayerInputLink::sentActions() const {
  return sentActions_;
}
