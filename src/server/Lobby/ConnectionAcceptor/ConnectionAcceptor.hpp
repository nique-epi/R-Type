#pragma once

#include <cstdint>
#include <functional>
#include <random>
#include <span>
#include "ConnectedPlayer.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "CookieIssuer.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "Logger.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
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
 * answer, whoever sent it, and an unknown sender never gets more bytes than it
 * sent (LARGEST_CONNECTION_ANSWER_DATAGRAM_SIZE). Every other message is
 * ignored, so an unknown sender gets nothing but answers to its requests. A
 * request is answered as follows, in this order:
 *
 * - from a connected player: the same ConnectionAccepted again, same number
 *   and same session token, when the cookie is valid; a ConnectionChallenge
 *   otherwise, even while a game is in progress;
 * - while a game is in progress: ConnectionRefused (GameInProgress);
 * - when MAXIMUM_PLAYERS players are connected: ConnectionRefused (GameFull);
 * - without a valid cookie: a ConnectionChallenge, and nothing is kept;
 * - with a valid cookie: a place in the roster, a session token drawn at
 *   random, and a ConnectionAccepted that carries both; the new player is
 *   announced to admit once.
 *
 * A refusal reserves nothing, so it is sent before the cookie is checked.
 * Nothing caused by the content of a datagram makes it throw.
 */
class ConnectionAcceptor {
 public:
  /**
   * @brief What the acceptor calls for each answer to send.
   */
  using Send = std::function<void(network::OutgoingDatagram)>;

  /**
   * @brief What the acceptor calls once for each sender it gives a place, so
   * the datagrams of that player reach the game from then on.
   */
  using Admit = std::function<void(const network::Endpoint&)>;

  /**
   * @param roster The players of the game; it must outlive the acceptor.
   * @param send Must not be empty.
   * @param admit Must not be empty.
   */
  ConnectionAcceptor(PlayerRoster& roster, Send send, Admit admit);

  /**
   * @brief Answers the requests among @p datagrams, oldest first.
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
  void answer(const network::Endpoint& sender,
              const network::ConnectionRequest& request, engine::Duration now);
  void accept(ConnectedPlayer& player);
  void challenge(const network::Endpoint& sender, engine::Duration now);
  void refuse(const network::Endpoint& sender, network::RefusalReason reason);

  /** @returns A session token drawn at random, never 0. */
  [[nodiscard]] std::uint64_t drawSessionToken();

  PlayerRoster* roster_;
  Send send_;
  Admit admit_;
  network::CookieIssuer cookies_;
  std::random_device random_;
  bool gameInProgress_{false};
  logging::Logger logger_;
};

}  // namespace rtype::server
