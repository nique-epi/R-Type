#include "RecordingSessionOutbox.hpp"
#include <cstddef>
#include <span>
#include <vector>
#include "MessageType.hpp"
#include "SentToServer.hpp"

void RecordingSessionOutbox::sendToServer(rtype::network::MessageType type,
                                          std::span<const std::byte> payload) {
  sent_.push_back(SentToServer{
      .type = type,
      .payload = std::vector<std::byte>(payload.begin(), payload.end())});
}

bool RecordingSessionOutbox::isAcknowledgementDue() const { return false; }

const std::vector<SentToServer>& RecordingSessionOutbox::sent() const {
  return sent_;
}
