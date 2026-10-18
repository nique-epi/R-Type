#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "BoundedQueue.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"

namespace rtype::server {

/**
 * @brief Sorts each datagram the network thread receives before any queue
 * holds it, so that a flood from senders that were not admitted, or that do
 * not know the session token of the player whose address they use, cannot
 * push the datagrams of the players out of their queue. A player that floods
 * with its own token still can: the players share one queue.
 *
 * A datagram whose header is of this protocol and version, with no flag set,
 * and whose messages add up to its end, is kept only if, coming from the
 * address and port of an admitted player, it carries the session token of
 * that player, or NO_SESSION_TOKEN when it holds a ConnectionRequest; any
 * other token drops it, counted apart from the other drops. A datagram kept
 * goes:
 *
 * - to the connection request queue when it holds a ConnectionRequest: a
 *   player whose acceptance was lost asks again;
 * - to the player queue when its sender was admitted;
 * - nowhere otherwise.
 *
 * Every other datagram is dropped. The sorter reads the header and the
 * type of each message, never a payload, and throws nothing on a malformed
 * datagram, so its cost per datagram stays close to that of the copy into a
 * queue, whatever the datagram holds. Not thread-safe, except droppedCount()
 * and wrongTokenCount(): call the rest on the network thread only.
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
   * @brief From now on, the datagrams of @p player that carry
   * @p sessionToken go to the player queue. Admitting a player again keeps
   * one entry and replaces its token. The callers admit at most the players of
   * the game, so the list stays as small as the game.
   */
  void admit(const network::Endpoint& player, std::uint64_t sessionToken);

  /**
   * @returns How many datagrams sort() dropped since the sorter was built,
   * those with a wrong session token apart. Call it from any thread.
   */
  [[nodiscard]] std::size_t droppedCount() const;

  /**
   * @returns How many datagrams sort() dropped since the sorter was built
   * because they came from the address of an admitted player with another
   * session token. Call it from any thread.
   */
  [[nodiscard]] std::size_t wrongTokenCount() const;

 private:
  /** @brief A player whose datagrams reach the player queue. */
  struct AdmittedPlayer {
    network::Endpoint endpoint;
    std::uint64_t sessionToken{};
  };

  [[nodiscard]] const AdmittedPlayer* findAdmitted(
      const network::Endpoint& sender) const;
  void drop();

  Destinations destinations_;
  std::vector<AdmittedPlayer> admitted_;
  std::atomic<std::size_t> droppedCount_{0};
  std::atomic<std::size_t> wrongTokenCount_{0};
};

}  // namespace rtype::server
