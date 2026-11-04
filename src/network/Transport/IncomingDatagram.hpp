#pragma once

#include <chrono>
#include <cstddef>
#include <vector>
#include "Endpoint.hpp"

namespace rtype::network {

/**
 * @brief A datagram a socket received, copied out of the receive buffer: who
 * sent it, its bytes, and when it arrived.
 */
struct IncomingDatagram {
  Endpoint sender;
  std::vector<std::byte> payload;
  /** @brief When the socket handed the datagram over, as the time since the
   * epoch of std::chrono::steady_clock, the clock engine::SystemClock reads.
   * It is read on the network thread, so it leaves out the time the datagram
   * then waits for the frame or the tick that reads it. 0 for a datagram
   * built by hand. */
  std::chrono::nanoseconds receivedAt{};

  bool operator==(const IncomingDatagram&) const = default;
};

}  // namespace rtype::network
