#pragma once

#include <cstddef>
#include <mutex>
#include <span>
#include "Endpoint.hpp"
#include "TrafficReport.hpp"
#include "TwoWayTraffic.hpp"

namespace rtype::network {

/**
 * @brief Counts the datagrams a socket sends and receives, by peer and by type
 * of message, in totals that only grow.
 *
 * Only the peers given to follow() get a row, so what a stranger sends costs no
 * memory, whatever address it claims: it goes in TrafficReport::others. Safe
 * from any thread: the lock is held to add one datagram or to copy the report.
 */
class TrafficCounters {
 public:
  /** @brief Adds @p datagram, handed to the socket for @p destination. */
  void countSent(const Endpoint& destination,
                 std::span<const std::byte> datagram);

  /** @brief Adds @p datagram, received from @p sender. Throws nothing caused by
   * what it holds. */
  void countReceived(const Endpoint& sender,
                     std::span<const std::byte> datagram);

  /** @brief From now on, the datagrams exchanged with @p peer go in a row of
   * its own. A peer already followed keeps its row. */
  void follow(const Endpoint& peer);

  /** @brief Moves the row of @p peer to TrafficReport::departed; a peer not
   * followed changes nothing. */
  void unfollow(const Endpoint& peer);

  /** @returns A copy of what was counted so far. */
  [[nodiscard]] TrafficReport report() const;

 private:
  /** @returns The row of @p peer, or the row of the others. */
  [[nodiscard]] TwoWayTraffic& trafficOf(const Endpoint& peer);

  mutable std::mutex mutex_;
  TrafficReport counted_;
};

}  // namespace rtype::network
