#pragma once

#include <cstdint>
#include <functional>
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectionAcceptorParts.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "CookieIssuer.hpp"
#include "Endpoint.hpp"
#include "EntityTypeEntry.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyConstants.hpp"
#include "Logger.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "RequestReader.hpp"
#include "ThrottledLog.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Answers the connection requests among the datagrams the server
 * received, as the Connection section of the protocol page describes: with a
 * challenge, an acceptance or a refusal.
 *
 * A datagram is read only when its header carries this protocol and version
 * with no flag set, and its messages add up to its end. Only its first
 * ConnectionRequest is answered, so a datagram never gets more than one
 * answer, whoever sent it, and a sender that did not prove its address never
 * gets more bytes than it sent (LARGEST_UNPROVEN_ANSWER_DATAGRAM_SIZE). Every
 * other message is ignored, so an unknown sender gets nothing but answers to
 * its requests. A datagram that is not read, or whose first request its reader
 * refuses, gets no answer; it is logged at the debug level with its sender and
 * why, at most once per REJECTION_LINE_INTERVAL (see ThrottledLog). A request
 * is answered as follows, in this order:
 *
 * - from a connected player: a ConnectionChallenge when the cookie is not
 *   valid, even while a game is in progress; with a valid cookie, the same
 *   ConnectionAccepted again, same number, same session token and same table
 *   of entity types, once CONNECTION_ACCEPTANCE_REPEAT_INTERVAL passed since
 *   the previous acceptance of that player, and nothing before;
 * - while a game is in progress: ConnectionRefused (GameInProgress);
 * - when MAXIMUM_PLAYERS players are connected: ConnectionRefused (GameFull);
 * - when a player holds a seat under the same nickname, away or not, letters
 *   of the ASCII alphabet compared without regard to case: ConnectionRefused
 *   (NicknameTaken);
 * - without a valid cookie: a ConnectionChallenge, and nothing is kept;
 * - with a valid cookie: a place in the roster, a session token drawn at
 *   random, and a ConnectionAccepted that carries both and the entity types of
 *   the game; the new player is announced to admit once.
 *
 * A refusal reserves nothing, so it is sent before the cookie is checked. Only
 * a request with a valid cookie gets an acceptance, which is larger than the
 * request: the cookie went to the address the request came from, so only the
 * owner of that address could send it back, and the acceptance goes to that
 * address. Nothing caused by the content of a datagram makes it throw.
 *
 * A datagram whose header carries this protocol with another version is
 * answered with ConnectionRefused (IncompatibleVersion), whatever it holds,
 * when it is at least CONNECTION_REFUSED_DATAGRAM_SIZE bytes long, so the
 * refusal is never larger than what it answers; a shorter one gets nothing.
 * The DatagramSorter hands over such a datagram only from a sender it did not
 * admit.
 */

class ConnectionAcceptor {
 public:
  /** @param parts The roster, how to send, how to admit an address, and whom
   * to tell of a player who joined. */
  explicit ConnectionAcceptor(ConnectionAcceptorParts parts);

  /**
   * @brief Answers the requests among @p datagrams, oldest first.
   *
   * An exception answering one (any std::exception, from the roster, the
   * admission, the report of a joined player or the drawing of a token) is a
   * fault of the server, not of the datagram: it is logged at the error level
   * with its sender at most once per REJECTION_LINE_INTERVAL, that datagram is
   * dropped, and the next one is answered. What the answer changed before it
   * threw is kept.
   *
   * @param now Measured from any fixed origin, the same for every call: the
   * cookies depend on it.
   */
  void receive(std::span<const network::IncomingDatagram> datagrams,
               engine::Duration now);

  /**
   * @brief Sets whether a game is in progress; while one is, a sender that is
   * not connected yet is refused with GameInProgress. No game is in progress
   * when the acceptor is built.
   */
  void setGameInProgress(bool inProgress);

 private:
  /** @brief Refuses @p datagram, of another protocol version, as
   * IncompatibleVersion, or drops it when it is shorter than
   * CONNECTION_REFUSED_DATAGRAM_SIZE. */
  void refuseAnotherVersion(const network::IncomingDatagram& datagram,
                            engine::Duration now);
  void answer(const network::Endpoint& sender,
              const network::ConnectionRequest& request, engine::Duration now);
  /** @brief Sends @p player its ConnectionAccepted, with the entity types of
   * the game, and notes @p now as the time of its last acceptance. */
  void accept(ConnectedPlayer& player, engine::Duration now);
  void challenge(const network::Endpoint& sender, engine::Duration now);
  void refuse(const network::Endpoint& sender, network::RefusalReason reason);

  /** @returns A session token drawn at random, never 0. */
  [[nodiscard]] std::uint64_t drawSessionToken();

  PlayerRoster* roster_;
  std::function<void(network::OutgoingDatagram)> send_;
  std::function<void(const network::Endpoint&, std::uint64_t)> admit_;
  std::function<void(ConnectedPlayer&)> onPlayerJoined_;
  std::vector<network::EntityTypeEntry> entityTypes_;
  network::CookieIssuer cookies_;
  std::random_device random_;
  bool gameInProgress_{false};
  logging::Logger logger_;
  RequestReader<network::ConnectionRequest> requests_{
      std::string{LOBBY_LOGGER_NAME}, NO_REQUEST_DROP_REASON};
  logging::ThrottledLog failureLog_{std::string{LOBBY_LOGGER_NAME},
                                    network::REJECTION_LINE_INTERVAL};
};

}  // namespace rtype::server
