#pragma once

#include <cstddef>
#include <span>
#include <vector>
#include "BoundedQueue.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"

namespace rtype::server {

/**
 * @brief Sorts each datagram the network thread receives before any queue
 * holds it, so that a flood of strangers never pushes the datagrams of the
 * players out of their queue.
 *
 * A datagram whose header is of this protocol and version, with no flag set,
 * and whose messages add up to its end, goes:
 *
 * - to the connection request queue when it holds a ConnectionRequest,
 *   whoever sent it: a player whose acceptance was lost asks again;
 * - to the player queue when its sender was admitted;
 * - nowhere otherwise.
 *
 * Every other datagram is dropped. The sorter reads the header and the type
 * of each message, never a payload, so its cost per datagram stays close to
 * that of the copy into a queue. Not thread-safe: call it on the network
 * thread only.
 */
class DatagramSorter {
 public:
  /**
   * @brief The queues the datagrams go to, named so they cannot be swapped by
   * mistake. Both must outlive the sorter.
   */
  struct Destinations {
    concurrency::BoundedQueue<network::IncomingDatagram>* players{};
    concurrency::BoundedQueue<network::IncomingDatagram>* connectionRequests{};
  };

  /** @param destinations Neither queue may be null. */
  explicit DatagramSorter(Destinations destinations);

  /**
   * @brief Puts a copy of @p datagram, sent by @p sender, in the queue it
   * belongs to, or drops it. Nothing caused by its content makes it throw.
   */
  void sort(const network::Endpoint& sender,
            std::span<const std::byte> datagram);

  /**
   * @brief From now on, the datagrams of @p player go to the player queue.
   * Admitting a player again changes nothing. The callers admit at most the
   * players of the game, so the list stays as small as the game.
   */
  void admit(const network::Endpoint& player);

 private:
  [[nodiscard]] bool isAdmitted(const network::Endpoint& sender) const;

  Destinations destinations_;
  std::vector<network::Endpoint> admitted_;
};

}  // namespace rtype::server
