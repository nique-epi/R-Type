#pragma once

#include <vector>
#include "PeerTraffic.hpp"
#include "TwoWayTraffic.hpp"

namespace rtype::network {

/**
 * @brief What TrafficCounters counted since the program started, in rows that
 * never overlap: a datagram is in exactly one.
 */
struct TrafficReport {
  /** @brief The peers followed now, in the order they were first followed. */
  std::vector<PeerTraffic> peers;
  /** @brief Anyone else: on the server, what comes from or goes to an address
   * that holds no place; on the client, which follows nobody, everything. */
  TwoWayTraffic others;
  /** @brief The peers followed in the past, added up. */
  TwoWayTraffic departed;
};

}  // namespace rtype::network
