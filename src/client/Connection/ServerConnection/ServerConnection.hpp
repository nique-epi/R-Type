#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ActionSet.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "EntityMessage.hpp"
#include "GameEventHistory.hpp"
#include "IDatagramLink.hpp"
#include "IEntityMessageSource.hpp"
#include "IPlayerInputLink.hpp"
#include "IServerConnection.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyState.hpp"
#include "Logger.hpp"
#include "MessageDispatcher.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "PacketHeader.hpp"
#include "PendingDeparture.hpp"
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The attempt of the client to join a server, as the Connection section
 * of the protocol page describes, then the session the server opens, and
 * the inputs of the game it sends there.
 *
 * connect() sends a ConnectionRequest without a cookie at once; while the
 * server does not answer, the request goes out again every
 * CONNECTION_REQUEST_INTERVAL. A ConnectionChallenge is answered at once with
 * the request carrying its cookie, and every later request carries it too. A
 * ConnectionAccepted or a ConnectionRefused ends the attempt, and no request is
 * sent any more. An attempt that got neither within CONNECTION_ATTEMPT_TIMEOUT
 * of connect() gives up as Unreachable: a challenge does not restart that
 * delay.
 *
 * Once accepted, the connection keeps the session token of the acceptance and
 * writes it into every datagram it sends; every datagram takes its sequence
 * from the same counter, which starts at 0. Of the datagrams of the server, it
 * reads only those that carry that token, then only those newer than the
 * newest one it read, so a late LobbyState never undoes a newer one. It keeps
 * the players of the newest LobbyState, reads a Heartbeat as nothing more than
 * a datagram of the server, and learns that the game started from
 * a GameStarted, or from a WorldState when the GameStarted was lost. setReady()
 * sends a ReadyState at once, then again every READY_STATE_INTERVAL while the
 * waiting room shows another value. While the attempt is Accepted,
 * sendPlayerInput() sends the inputs of the game, numbered from
 * FIRST_INPUT_NUMBER; otherwise it sends nothing. While the attempt is
 * Accepted, a Heartbeat goes out once HEARTBEAT_INTERVAL passed without any
 * other datagram to the server, so the server knows the player is still there
 * while nothing else is due. A session whose server sent nothing for
 * SESSION_TIMEOUT is lost: the status becomes Lost, the session is forgotten
 * and nothing more goes to the server, not even a Disconnection. Every datagram
 * of the server that the checks of this comment let through counts, whatever
 * it holds: from that server, of this protocol and version, with the token of
 * the session, newer than the newest one read, and whose messages add up to its
 * end. The time counts before update() reads what arrived, so a frame longer
 * than SESSION_TIMEOUT loses no session whose datagrams waited in the link.
 * Leaving the session, through
 * cancel() or connect(), sends a Disconnection DISCONNECTION_COPY_COUNT times,
 * DISCONNECTION_COPY_INTERVAL apart, with the token and the sequences of the
 * session left, the first at once and the others from update(), while
 * isLeaving() is true; connect() then starts a new session: sequence 0, token 0
 * and inputs from FIRST_INPUT_NUMBER again.
 *
 * From the start of the game, it keeps the EntitySpawned, WorldState and
 * EntityDestroyed of the server, and the first copy of each GameEvent (see
 * GameEventHistory), in the order they arrived, until drainEntityMessages()
 * hands them over; leaving the session drops those not handed over and
 * forgets the events seen. The game screen drains them every frame, behind
 * the options too, so they never pile up for longer than the loading of the
 * game.
 *
 * Only datagrams from the server being contacted, of this protocol and version
 * and whose messages add up to their end, are read; any other is ignored, with
 * one exception: during an attempt, a datagram of that server whose header is
 * of this protocol with another version ends the attempt as Refused
 * (IncompatibleVersion), with the version the server wrote. Within a session,
 * such a datagram is ignored, so a forged one cannot end it.
 * Once accepted, the connection keeps the session token the acceptance
 * carries, and drops and counts any later datagram of the server that carries
 * another one.
 *
 * update() reads what arrived and sends what is due, once per frame;
 * sendPlayerInput() sends at once; drainEntityMessages() hands over what
 * update() kept. Call them all on the thread of the frame loop.
 */
class ServerConnection : public IServerConnection,
                         public IPlayerInputLink,
                         public IEntityMessageSource {
 public:
  /**
   * @param link Where the datagrams go and come from; it must outlive the
   * connection.
   */
  explicit ServerConnection(IDatagramLink& link);

  /**
   * @throws InvalidNicknameException when @p nickname is not 1 to
   * MAXIMUM_NICKNAME_SIZE bytes of UTF-8; the attempt in progress, if any, is
   * dropped then.
   */
  void connect(const network::Endpoint& server,
               std::string_view nickname) override;
  void cancel() override;
  [[nodiscard]] ConnectionStatus status() const override;
  [[nodiscard]] std::span<const network::LobbyPlayer> lobbyPlayers()
      const override;
  void setReady(bool ready) override;
  [[nodiscard]] std::optional<bool> requestedReady() const override;
  [[nodiscard]] bool gameStarted() const override;

  /**
   * @brief Reads the datagrams received since the previous call, then sends
   * what is due: the request of an attempt, or its end; a ReadyState the
   * waiting room does not show yet; a copy of a Disconnection; a Heartbeat
   * once HEARTBEAT_INTERVAL passed in a session without sending anything.
   * A session whose server sent nothing for SESSION_TIMEOUT ends as Lost
   * instead, and nothing goes to it.
   *
   * @param elapsed Time since the previous call, or since connect().
   */
  void update(engine::Duration elapsed);

  void sendPlayerInput(ActionSet held) override;

  void drainEntityMessages(std::vector<EntityMessage>& destination) override;

  /**
   * @returns How many datagrams of the server were dropped since the
   * connection was built because they came after an acceptance with another
   * session token than the one it carried.
   */
  [[nodiscard]] std::size_t wrongTokenCount() const;

  /**
   * @returns Whether copies of a Disconnection are still due to a server the
   * player left: update() sends one each DISCONNECTION_COPY_INTERVAL until
   * DISCONNECTION_COPY_COUNT went out.
   */
  [[nodiscard]] bool isLeaving() const;

 private:
  void registerAnswers();
  void registerSessionMessages();

  /** @brief Reads the datagrams the link received since the previous call. */
  void readReceived();
  void receive(const network::IncomingDatagram& datagram);

  /** @brief Gives the session up as Lost once SESSION_TIMEOUT passed without a
   * datagram of the server, before anything is sent; otherwise sends what the
   * session needs: the ReadyState the waiting room does not show yet, and a
   * Heartbeat once HEARTBEAT_INTERVAL passed without sending. The elapsed time
   * counts before either is sent, so a datagram sent now starts the next
   * delay. */
  void holdSession(engine::Duration elapsed);

  /** @brief Gives the attempt up as Unreachable once CONNECTION_ATTEMPT_TIMEOUT
   * passed since connect(), and otherwise sends the request again every
   * CONNECTION_REQUEST_INTERVAL. */
  void continueAttempt(engine::Duration elapsed);

  /** @brief Ends the attempt as Unreachable, or the session as Lost, as
   * @p ending says; forgets the session, sends nothing, and logs that the
   * server did not answer for @p silence. */
  void giveUp(ConnectionStatus::Phase ending, std::chrono::seconds silence);

  /** @brief Ends the attempt in progress as Refused (IncompatibleVersion),
   * since the server answered in @p serverVersion, which it keeps, and logs
   * both versions; sends nothing, and does nothing outside an attempt. */
  void endAttemptForServerVersion(std::uint8_t serverVersion);

  /**
   * @returns Whether a datagram of the server may be read: always before the
   * acceptance; then only with the token of the session, a datagram with
   * another token being counted by wrongTokenCount(), and newer than the
   * newest one read, which it becomes.
   */
  [[nodiscard]] bool passesSessionChecks(const network::PacketHeader& header);

  /** @brief Notes that the game started, once per session, and forgets the
   * ready state asked for, which belonged to the waiting room. */
  void startGame();

  /** @brief Keeps an entity message of the server until the next drain, once
   * the game started; before, nothing is kept, since the first WorldState of
   * the game lists every entity anyway. */
  void keepEntityMessage(EntityMessage message);

  void sendRequest();
  void sendReadyState(bool ready);

  /** @brief Sends the ReadyState again once READY_STATE_INTERVAL passed while
   * the waiting room shows another value than the one asked for. */
  void repeatReadyState(engine::Duration elapsed);

  /** @brief Sends a Heartbeat once HEARTBEAT_INTERVAL passed since the last
   * datagram to the server. */
  void sendHeartbeatWhenSilent();

  /** @brief Leaves the session: the first copy of the Disconnection goes at
   * once, the others in the next updates. */
  void leave();
  void sendDepartureCopy();
  void repeatDeparture(engine::Duration elapsed);

  /** @brief Forgets the token, the sequences, the waiting room, the ready
   * state asked for, the start of the game and the entity messages not handed
   * over. */
  void forgetSession();

  /** @brief Sends the server one datagram holding @p payload as a message of
   * type @p type, under nextHeader(): every datagram to the server goes
   * through here, so each one starts the delay before the next Heartbeat
   * over. */
  void sendToServer(network::MessageType type,
                    std::span<const std::byte> payload);

  /** @brief Sends the server one datagram holding @p message, as the overload
   * above does.
   * @throws What encodeMessage() throws for @p message, before anything is
   * sent or any sequence is used. */
  template <network::EncodableMessage Message>
  void sendToServer(const Message& message) {
    sendToServer(Message::TYPE, network::encodeMessage(message));
  }

  /** @returns A header carrying the token of the session, 0 before it, and
   * the next sequence, which it uses up. Every datagram to the server takes
   * its header here, so the delay before the next Heartbeat starts again from
   * it. */
  [[nodiscard]] network::PacketHeader nextHeader();

  /** @returns A header carrying @p token and @p sequence, which then moves on
   * to the next one. */
  [[nodiscard]] static network::PacketHeader headerOf(std::uint64_t token,
                                                      std::uint16_t& sequence);

  [[nodiscard]] bool isConnecting() const;
  [[nodiscard]] bool isAccepted() const;

  /** @returns Whether the player's line of the waiting room shows @p ready. */
  [[nodiscard]] bool roomShowsReady(bool ready) const;

  IDatagramLink* link_;
  network::MessageDispatcher dispatcher_;
  logging::Logger logger_;
  network::Endpoint server_;
  std::string nickname_;
  std::uint64_t cookie_{0};
  std::uint64_t sessionToken_{network::NO_SESSION_TOKEN};
  std::size_t wrongTokenCount_{0};
  std::uint16_t nextSequence_{0};
  std::uint16_t newestReceivedSequence_{0};
  std::uint32_t nextInputNumber_{network::FIRST_INPUT_NUMBER};
  engine::Duration sinceRequest_{};
  engine::Duration sinceConnect_{};
  engine::Duration sinceReadyState_{};
  engine::Duration sinceSent_{};
  engine::Duration sinceReceived_{};
  ConnectionStatus status_;
  std::vector<network::LobbyPlayer> lobbyPlayers_;
  std::optional<bool> requestedReady_;
  bool gameStarted_ = false;
  std::optional<PendingDeparture> departure_;
  std::vector<network::IncomingDatagram> received_;
  std::vector<EntityMessage> entityMessages_;
  GameEventHistory gameEvents_;
};

}  // namespace rtype::client
