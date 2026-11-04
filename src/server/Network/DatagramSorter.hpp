#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "AdmittedPlayer.hpp"
#include "DatagramDrops.hpp"
#include "DatagramQueues.hpp"
#include "Endpoint.hpp"
#include "PacketHeader.hpp"

namespace rtype::server {

/**
 * @brief Sorts each datagram the network thread receives before any queue
 * holds it, so that a flood from senders that were not admitted, or that do
 * not know the session token of the player whose address they use, cannot
 * push the datagrams of the players out of their queue. A player that floods
 * with its own token still can: the players share one queue.
 *
 * A datagram whose header is of this protocol and version, with no flag this
 * version does not know, and whose messages add up to its end, is kept only
 * if, coming from the address and port of an admitted player, it carries the
 * session token of that player, or NO_SESSION_TOKEN when it holds a
 * ConnectionRequest; any other token drops it, counted apart from the other
 * drops. A datagram kept goes:
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
 * than what it answers. Every other datagram is dropped, counted and logged by
 * the DatagramDrops the sorter is given. The sorter reads the header and the
 * type of each message, never a payload, throws nothing on a malformed
 * datagram, and the clock is read only to drop one, so its cost per datagram
 * stays close to that of the copy into a queue, whatever the datagram holds.
 * Not thread-safe: call it on the network thread only.
 */
class DatagramSorter {
 public:
  /**
   * @param destinations Neither queue may be null.
   * @param drops Where the datagrams dropped are counted and logged; it must
   * outlive the sorter.
   */
  DatagramSorter(DatagramQueues destinations, DatagramDrops& drops);

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
   * @returns Whether @p player held a place under @p sessionToken, so is
   * forgotten now.
   */
  bool forget(const network::Endpoint& player, std::uint64_t sessionToken);

 private:
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

  DatagramQueues destinations_;
  DatagramDrops* drops_;
  std::vector<AdmittedPlayer> admitted_;
};

}  // namespace rtype::server
