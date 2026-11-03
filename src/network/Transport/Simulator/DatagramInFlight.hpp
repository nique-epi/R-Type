#pragma once

#include <chrono>
#include <cstddef>
#include <vector>
#include "Endpoint.hpp"

namespace rtype::network {

/**
 * @brief A datagram crossing the simulated network: when it arrives, the
 * other end of its way (its destination when the program sends it, its sender
 * when the program receives it), and its bytes.
 */
struct DatagramInFlight {
  std::chrono::steady_clock::time_point arrival;
  Endpoint peer;
  std::vector<std::byte> payload;

  bool operator==(const DatagramInFlight&) const = default;
};

}  // namespace rtype::network
