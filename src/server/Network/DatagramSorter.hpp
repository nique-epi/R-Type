#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "AdmittedPlayer.hpp"
#include "AllowanceVerdict.hpp"
#include "DatagramDrops.hpp"
#include "DatagramQueues.hpp"
#include "Endpoint.hpp"
#include "IClock.hpp"
#include "PacketHeader.hpp"
#include "RateLimitConstants.hpp"
#include "RequestAllowances.hpp"
#include "SpokenDatagram.hpp"

namespace rtype::server {

/**
 * @brief Sorts each datagram the network thread receives before any queue
 * holds it, so that a flood from senders that were not admitted, from senders
 * that do not know the session token of the player whose address they use,
 * or from a sender over its rate limit, cannot push the datagrams of the
 * players out of their queue.
 *
 * A datagram whose header is of this protocol and version, with no flag this
 * version does not know, and whose messages add up to its end, is kept only
 * if, coming from the address and port of an admitted player, it carries the
 * session token of that player, or NO_SESSION_TOKEN when it holds a
 * ConnectionRequest; any other token drops it, counted apart from the other
 * drops. Then it spends an allowance: the one of its player, under
 * PLAYER_RATE_LIMIT, when it carries that player's token; otherwise, so for a
 * ConnectionRequest of a stranger or one with NO_SESSION_TOKEN, the one its
 * address and port hold among the requests, under REQUEST_RATE_LIMIT. The
 * token is checked before any allowance is spent, so whoever forges the
 * address of a player never spends that player's allowance. A datagram over
 * its limit, or sent during a ban, is dropped. A datagram kept goes:
 *
 * - to the connection request queue when it holds a ConnectionRequest: a
 *   player whose acceptance was lost asks again;
 * - to the player queue when its sender was admitted;
 * - nowhere otherwise, without spending anything.
 *
 * A datagram whose header is of another version goes to the connection request
 * queue, where the ConnectionAcceptor refuses it as IncompatibleVersion, when
 * its sender was not admitted, it holds at least
 * CONNECTION_REFUSED_DATAGRAM_SIZE bytes, so the refusal is never larger than
 * what it answers, and it is within the allowance of its address and port
 * among the requests. Every other datagram is dropped, counted and logged by
 * the DatagramDrops the sorter is given. The sorter reads the header and the
 * type of each message, never a payload, throws nothing on a malformed
 * datagram, and reads the clock only for a datagram that passed the token, so
 * its cost per datagram stays close to that of the copy into a queue, whatever
 * the datagram holds. Not thread-safe: call it on the network thread only.
 */
class DatagramSorter {
 public:
  /**
   * @param destinations Neither queue may be null.
   * @param drops, clock Where the datagrams dropped are counted and logged,
   * and the time the allowances are spent at; both must outlive the sorter.
   */
  DatagramSorter(DatagramQueues destinations, DatagramDrops& drops,
                 const engine::IClock& clock);

  /**
   * @brief Puts a copy of @p datagram, sent by @p sender, in the queue it
   * belongs to, or drops it. Nothing caused by its content makes it throw.
   */
  void sort(const network::Endpoint& sender,
            std::span<const std::byte> datagram);

  /**
   * @brief From now on, the datagrams of @p player that carry
   * @p sessionToken go to the player queue. Admitting a player again keeps
   * one entry: a new token replaces the old one with a fresh allowance, and
   * the same token keeps what its datagrams spent, so asking to join again
   * never lifts a ban. The callers admit at most the players of the game, and
   * forget each one that leaves, so the list stays as small as the game.
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
  [[nodiscard]] AdmittedPlayer* findAdmitted(const network::Endpoint& sender);

  /** @brief Sends @p datagram, whose header this code does not speak, to the
   * connection request queue when isRefusedForItsVersion() and within the
   * allowance of its sender, and drops it otherwise. */
  void sortForeignHeader(const network::Endpoint& sender,
                         const network::PacketHeader& header,
                         std::span<const std::byte> datagram);

  /** @returns Whether the ConnectionAcceptor must refuse a datagram of
   * @p datagramSize bytes as IncompatibleVersion: its header is of another
   * version, @p sender was not admitted, and it holds at least
   * CONNECTION_REFUSED_DATAGRAM_SIZE bytes, so the refusal is never larger. */
  [[nodiscard]] bool isRefusedForItsVersion(const network::Endpoint& sender,
                                            const network::PacketHeader& header,
                                            std::size_t datagramSize);

  /** @brief Checks the token of @p datagram, spends its allowance, then puts
   * a copy of it in its queue or drops it. */
  void sortSpoken(const SpokenDatagram& datagram);

  /** @returns The verdict of the allowance @p datagram spends: the one of
   * @p player when it carries the token of @p player, the one of its address
   * and port among the requests otherwise. */
  [[nodiscard]] network::AllowanceVerdict spendAllowanceOf(
      const SpokenDatagram& datagram, AdmittedPlayer* player);

  /** @returns Whether @p verdict lets the datagram of @p sender through; drops
   * it, with the reason, otherwise. */
  [[nodiscard]] bool keptByLimit(network::AllowanceVerdict verdict,
                                 const network::Endpoint& sender);

  DatagramQueues destinations_;
  DatagramDrops* drops_;
  const engine::IClock* clock_;
  std::vector<AdmittedPlayer> admitted_;
  network::RequestAllowances requests_{network::REQUEST_RATE_LIMIT};
};

}  // namespace rtype::server
