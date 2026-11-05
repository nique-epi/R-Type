#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>
#include "AdmittedPlayer.hpp"
#include "AllowanceVerdict.hpp"
#include "DatagramQueues.hpp"
#include "Endpoint.hpp"
#include "IClock.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "RateLimitConstants.hpp"
#include "RejectionCause.hpp"
#include "RejectionLedger.hpp"
#include "RequestAllowances.hpp"
#include "SortedQueue.hpp"
#include "SpokenDatagram.hpp"
#include "ThrottledLog.hpp"

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
 * session token of that player, or NO_SESSION_TOKEN when it holds a message
 * sent outside any session, a ConnectionRequest or a ServerInformationRequest;
 * any other token drops it, counted apart from the other drops. Then it spends
 * an allowance: when it holds a message sent outside any session, whatever
 * token it carries, the one its address and port hold among the requests,
 * under REQUEST_RATE_LIMIT, so a player places no more requests in their
 * queues than a stranger, and a sender shares one limit between its
 * connection and information requests; otherwise the one of its player, under
 * PLAYER_RATE_LIMIT. The token is checked before any allowance is spent, so
 * whoever forges the address of a player never spends that player's
 * allowance. A datagram over
 * its limit, or sent during a ban, is dropped. A datagram kept goes:
 *
 * - to the connection request queue when it holds a ConnectionRequest: a
 *   player whose acceptance was lost asks again;
 * - to the information request queue when it holds a ServerInformationRequest
 *   and no ConnectionRequest, so a flood of information requests never pushes
 *   a connection request out of its queue;
 * - to the player queue when its sender was admitted;
 * - nowhere otherwise, without spending anything.
 *
 * A datagram whose header is of another version goes to the connection request
 * queue, where the ConnectionAcceptor refuses it as IncompatibleVersion, when
 * its sender was not admitted, it holds at least
 * CONNECTION_REFUSED_DATAGRAM_SIZE bytes, so the refusal is never larger than
 * what it answers, and it is within the allowance of its address and port
 * among the requests. Every other datagram is dropped.
 *
 * Each datagram dropped, and each datagram kept that made its queue discard the
 * oldest one, is counted in the RejectionLedger with its cause and its sender.
 * Each drop is also logged at the debug level with its sender and why, at most
 * once per REJECTION_LINE_INTERVAL (see ThrottledLog). A copy kept is stamped
 * with the time its datagram arrived. The sorter reads the header and the type
 * of each message, never a payload, throws nothing on a malformed datagram, and
 * reads the clock once for a datagram that passed the token, both to spend its
 * allowance and to stamp it, and once more to log a drop, so its cost per
 * datagram stays close to that of the copy into a queue, whatever the datagram
 * holds. Not thread-safe: call it on the network thread only.
 */
class DatagramSorter {
 public:
  /**
   * @param destinations None of the three queues may be null.
   * @param rejections Where each datagram rejected is counted; it must outlive
   * the sorter.
   * @param clock The time the allowances are spent at, which stamps each
   * datagram kept as the time it arrived, and the time of the lines about the
   * datagrams dropped; it must outlive the sorter.
   */
  DatagramSorter(DatagramQueues destinations, RejectionLedger& rejections,
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
   * the same token keeps what its datagrams spent, so admitting one session
   * twice never lifts its ban. The callers admit at most the players of the
   * game, and forget each one that leaves, so the list stays as small as the
   * game.
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
   * connection request queue when it is of another version of this protocol,
   * its sender was not admitted, it holds at least
   * CONNECTION_REFUSED_DATAGRAM_SIZE bytes, and it is within the allowance of
   * its sender, and drops it otherwise. */
  void sortForeignHeader(const network::Endpoint& sender,
                         const network::PacketHeader& header,
                         std::span<const std::byte> datagram);

  /** @brief Checks the token of @p datagram, spends its allowance, then puts
   * a copy of it in its queue or drops it. */
  void sortSpoken(const SpokenDatagram& datagram);

  /** @brief Puts @p copy, the datagram of @p sender, in @p queue, and counts
   * it against @p sender as the overflow of that queue (PlayerQueueFull,
   * RequestQueueFull or InformationQueueFull) when the queue discarded its
   * oldest. */
  void enqueue(SortedQueue queue, const network::Endpoint& sender,
               network::IncomingDatagram copy);

  /** @returns Whether @p verdict lets the datagram of @p sender through; drops
   * it, with the cause and the reason of the verdict, otherwise. */
  [[nodiscard]] bool keptByLimit(network::AllowanceVerdict verdict,
                                 const network::Endpoint& sender);

  /** @brief Counts the datagram of @p sender as rejected for @p cause, and
   * logs @p reason. */
  void drop(const network::Endpoint& sender, RejectionCause cause,
            std::string_view reason);

  /** @returns The verdict of the allowance @p datagram spends at @p now: the
   * one of its address and port among the requests when it holds a
   * message sent outside any session or comes from no admitted player, the one
   * of @p player otherwise. */
  [[nodiscard]] network::AllowanceVerdict spendAllowanceOf(
      const SpokenDatagram& datagram, AdmittedPlayer* player,
      std::chrono::nanoseconds now);

  DatagramQueues destinations_;
  RejectionLedger* rejections_;
  const engine::IClock* clock_;
  std::vector<AdmittedPlayer> admitted_;
  network::RequestAllowances requests_{network::REQUEST_RATE_LIMIT};
  logging::ThrottledLog dropLog_;
};

}  // namespace rtype::server
