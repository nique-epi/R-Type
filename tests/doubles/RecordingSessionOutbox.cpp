#include "RecordingSessionOutbox.hpp"
#include <algorithm>
#include <cstddef>
#include <span>
#include <utility>
#include <vector>
#include "MessageType.hpp"

void RecordingSessionOutbox::sendToServer(rtype::network::MessageType type,
                                          std::span<const std::byte> payload) {
  sent_.emplace_back(type,
                     std::vector<std::byte>(payload.begin(), payload.end()));
}

std::size_t RecordingSessionOutbox::sentCountOf(
    rtype::network::MessageType type) const {
  return static_cast<std::size_t>(std::ranges::count(
      sent_, type,
      &std::pair<rtype::network::MessageType, std::vector<std::byte>>::first));
}
