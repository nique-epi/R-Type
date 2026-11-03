#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
#include "BoundedQueue.hpp"
#include "Endpoint.hpp"
#include "IClock.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ThrottledLog.hpp"

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
 * A datagram whose header is of another version goes to the connection request
 * queue, where the ConnectionAcceptor refuses it as IncompatibleVersion, when
 * its sender was not admitted and it holds at least
 * CONNECTION_REFUSED_DATAGRAM_SIZE bytes: the refusal is then never larger
 * than what it answers. Every other datagram is dropped. Besides the counts the
 * server reports, each drop is logged at the debug level with its sender and
 * why, at most once per REJECTION_LINE_INTERVAL (see ThrottledLog). The sorter
 * reads the header and the type of each message, never a payload, throws
 * nothing on a malformed datagram, and reads the clock only to drop one, so its
 * cost per datagram stays close to that of the copy into a queue, whatever the
 * datagram holds. Not thread-safe, except droppedCount(),
 * wrongTokenCount() and takeWrongTokenSender(): call the rest on the network
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

  /**
   * @param destinations Neither queue may be null.
   * @param clock The time of the lines about the datagrams dropped; it must
   * outlive the sorter.
   */
  DatagramSorter(Destinations destinations, const engine::IClock& clock);

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
   * the game, and forget each one that leaves, so the list stays as small as
   * the game.
   */
  void admit(const network::Endpoint& player, std::uint64_t sessionToken);

  /**
   * @brief From now on, the datagrams of @p player are sorted like a
   * stranger's, unless it was admitted again since with another token than
   * @p sessionToken: forgetting a session that was replaced leaves the new one
   * alone, whatever order the two calls come in.
   */
  void forget(const network::Endpoint& player, std::uint64_t sessionToken);

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

  /**
   * @returns The sender of the first datagram dropped for another session
   * token since the previous call, or nothing when none was; the next call
   * starts afresh. The sender is kept before the drop is counted, so a call
   * made after wrongTokenCount() grew finds it, unless an earlier call took
   * it in between. Call it from any thread.
   */
  [[nodiscard]] std::optional<network::Endpoint> takeWrongTokenSender();

 private:
  /** @brief A player whose datagrams reach the player queue. */
  struct AdmittedPlayer {
    network::Endpoint endpoint;
    std::uint64_t sessionToken{};
  };

  [[nodiscard]] const AdmittedPlayer* findAdmitted(
      const network::Endpoint& sender) const;

  /** @brief Sends @p datagram, whose header this code does not speak, to the
   * connection request queue when isRefusedForItsVersion(), and drops it
   * otherwise. */
  void sortForeignHeader(const network::Endpoint& sender,
                         const network::PacketHeader& header,
                         std::span<const std::byte> datagram);

  /** @returns Whether the ConnectionAcceptor must refuse a datagram of
   * @p datagramSize bytes as IncompatibleVersion: its header is of another
   * version, @p sender was not admitted, and it holds at least
   * CONNECTION_REFUSED_DATAGRAM_SIZE bytes, so the refusal is never larger. */
  [[nodiscard]] bool isRefusedForItsVersion(const network::Endpoint& sender,
                                            const network::PacketHeader& header,
                                            std::size_t datagramSize) const;
  void drop(const network::Endpoint& sender, std::string_view reason);
  void dropWrongToken(const network::Endpoint& sender);

  Destinations destinations_;
  const engine::IClock* clock_;
  std::vector<AdmittedPlayer> admitted_;
  std::atomic<std::size_t> droppedCount_{0};
  std::atomic<std::size_t> wrongTokenCount_{0};
  std::mutex wrongTokenSenderMutex_;
  std::optional<network::Endpoint> wrongTokenSender_;
  logging::ThrottledLog dropLog_;
};

}  // namespace rtype::server
