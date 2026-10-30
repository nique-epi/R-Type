#pragma once

#include <cstdint>
#include <string_view>
#include "ActionSet.hpp"
#include "ConnectionAttempt.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "HandshakeAnswers.hpp"
#include "IPlayerInputLink.hpp"
#include "IServerConnection.hpp"
#include "Logger.hpp"
#include "MessageDispatcher.hpp"
#include "ProtocolConstants.hpp"
#include "ServerSessionParts.hpp"
#include "SessionDeparture.hpp"
#include "SessionInbound.hpp"
#include "SessionObservers.hpp"
#include "SessionOutbox.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The attempt of the client to join a server, as the Connection section
 * of the protocol page describes, and the session the server opens: the
 * handshake (see ConnectionAttempt), then the token, the sequences, the
 * heartbeats and the silence of the server (see SessionOutbox and
 * SessionInbound), and the departure (see SessionDeparture). The waiting room
 * and the entity messages of the game are other parts, told through
 * ISessionObserver and fed through the dispatcher.
 *
 * connect() sends a ConnectionRequest at once; a ConnectionAccepted or a
 * ConnectionRefused ends the attempt, and no request is sent any more. An
 * attempt that got neither within CONNECTION_ATTEMPT_TIMEOUT gives up as
 * Unreachable.
 *
 * Once accepted, the session keeps the token of the acceptance and writes it
 * into every datagram it sends. While the attempt is Accepted,
 * sendPlayerInput() sends the inputs of the game, numbered from
 * FIRST_INPUT_NUMBER; otherwise it sends nothing. A Heartbeat goes out once
 * HEARTBEAT_INTERVAL passed without any other datagram to the server. A session
 * whose server sent nothing for SESSION_SILENCE_LIMIT (SESSION_TIMEOUT plus
 * RECONNECTION_GRACE) is lost: the status becomes Lost, the session is
 * forgotten and nothing more goes to the server, not even a Disconnection.
 * Before that the session holds, with its token, and the first datagram of the
 * server that gets through gives the session back as it was. The time counts
 * before update() reads what arrived, so a long frame is never taken for
 * silence of the server. Leaving the session, through cancel() or connect(),
 * sends a Disconnection DISCONNECTION_COPY_COUNT times,
 * DISCONNECTION_COPY_INTERVAL apart, the first at once and the others from
 * update(), while isLeaving() is true.
 *
 * During an attempt, a datagram of the server whose header is of this protocol
 * with another version ends the attempt as Refused (IncompatibleVersion), with
 * the version the server wrote. Within a session, such a datagram is ignored,
 * so a forged one cannot end it.
 *
 * update() reads what arrived and sends what is due, once per frame;
 * sendPlayerInput() sends at once. Call them on the thread of the frame loop.
 */
class ServerSession final : public IServerConnection, public IPlayerInputLink {
 public:
  explicit ServerSession(ServerSessionParts parts);

  /**
   * @throws InvalidNicknameException when @p nickname is not 1 to
   * MAXIMUM_NICKNAME_SIZE bytes of UTF-8; the attempt in progress, if any, is
   * dropped then.
   */
  void connect(const network::Endpoint& server,
               std::string_view nickname) override;
  void cancel() override;
  [[nodiscard]] ConnectionStatus status() const override;

  /**
   * @brief Reads the datagrams received since the previous call, then sends
   * what is due: the request of an attempt, or its end; a copy of a
   * Disconnection; what the observers send; a Heartbeat once HEARTBEAT_INTERVAL
   * passed in a session without sending anything. A session whose server sent
   * nothing for SESSION_SILENCE_LIMIT ends as Lost instead, and nothing goes to
   * it.
   *
   * @param elapsed Time since the previous call, or since connect().
   */
  void update(engine::Duration elapsed);

  void sendPlayerInput(ActionSet held) override;

  /**
   * @returns Whether copies of a Disconnection are still due to a server the
   * player left: update() sends one each DISCONNECTION_COPY_INTERVAL until
   * DISCONNECTION_COPY_COUNT went out.
   */
  [[nodiscard]] bool isLeaving() const;

 private:
  /** @brief Reads the datagrams the link received since the previous call and
   * hands the messages of those that pass the checks to the dispatcher. */
  void readReceived();

  /** @brief Ends the attempt as Unreachable, or the session as Lost, as
   * @p ending says; forgets the session, sends nothing, and logs that the
   * server did not answer for the delay that ran out: SESSION_SILENCE_LIMIT for
   * Lost, CONNECTION_ATTEMPT_TIMEOUT for Unreachable. */
  void giveUp(ConnectionStatus::Phase ending);

  /** @brief Forgets the token, the sequences, and tells the observers the
   * session is closed. */
  void forgetSession();

  SessionOutbox outbox_;
  SessionInbound inbound_;
  SessionDeparture departure_;
  ConnectionAttempt attempt_;
  SessionObservers observers_;
  network::MessageDispatcher* dispatcher_;
  logging::Logger logger_;
  ConnectionStatus status_;
  HandshakeAnswers answers_;
  std::uint32_t nextInputNumber_{network::FIRST_INPUT_NUMBER};
};

}  // namespace rtype::client
