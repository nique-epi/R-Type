#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::network {

/** @brief The newest Heartbeat a RoundTripMeter received: its sentAt, to echo
 * it, and when its datagram arrived, to tell how long it was held. */
struct PeerStamp {
  std::uint32_t sentAt{};
  std::chrono::nanoseconds receivedAt{};
};

}  // namespace rtype::network
