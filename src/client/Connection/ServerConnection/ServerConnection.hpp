#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ActionSet.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "IDatagramLink.hpp"
#include "IPlayerInputLink.hpp"
#include "IServerConnection.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyState.hpp"
#include "Logger.hpp"
#include "MessageDispatcher.hpp"
#include "PacketHeader.hpp"
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
 * the players of the newest LobbyState, and learns that the game started from
 * a GameStarted, or from a WorldState when the GameStarted was lost. setReady()
 * sends a ReadyState at once, then again every READY_STATE_INTERVAL while the
 * waiting room shows another value. While the attempt is Accepted,
 * sendPlayerInput() sends the inputs of the game, numbered from
 * FIRST_INPUT_NUMBER; otherwise it sends nothing. Leaving the session, through
 * cancel() or connect(), sends a Disconnection DISCONNECTION_COPY_COUNT times,
 * DISCONNECTION_COPY_INTERVAL apart, with the token and the sequences of the
 * session left; connect() then starts a new session: sequence 0, token 0 and
 * inputs from FIRST_INPUT_NUMBER again.
 *
 * Only datagrams from the server being contacted, of this protocol and version
 * and whose messages add up to their end, are read; any other is ignored.
 *
 * update() reads what arrived and sends what is due, once per frame;
 * sendPlayerInput() sends at once. Call both on the thread of the frame loop.
 */
class ServerConnection : public IServerConnection, public IPlayerInputLink {
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
   * waiting room does not show yet; a copy of a Disconnection.
   *
   * @param elapsed Time since the previous call, or since connect().
   */
  void update(engine::Duration elapsed);

  void sendPlayerInput(ActionSet held) override;

 private:
  /** @brief The copies of a Disconnection still due to a server the player
   * left, sent with the token and the sequences of the session left. */
  struct Departure {
    network::Endpoint server;
    std::uint64_t sessionToken{};
    std::uint16_t nextSequence{};
    int copiesLeft{};
    engine::Duration sinceCopy{};
  };

  void registerAnswers();
  void registerSessionMessages();
  void receive(const network::IncomingDatagram& datagram);

  /**
   * @returns Whether a datagram of the server may be read: always before the
   * acceptance; then only with the token of the session, and newer than the
   * newest one read, which it becomes.
   */
  [[nodiscard]] bool passesSessionChecks(const network::PacketHeader& header);

  /** @brief Notes that the game started, once per session, and forgets the
   * ready state asked for, which belonged to the waiting room. */
  void startGame();

  void sendRequest();
  void sendReadyState(bool ready);

  /** @brief Sends the ReadyState again once READY_STATE_INTERVAL passed while
   * the waiting room shows another value than the one asked for. */
  void repeatReadyState(engine::Duration elapsed);

  /** @brief Leaves the session: the first copy of the Disconnection goes at
   * once, the others in the next updates. */
  void leave();
  void sendDepartureCopy();
  void repeatDeparture(engine::Duration elapsed);

  /** @brief Forgets the token, the sequences, the waiting room, the ready
   * state asked for and the start of the game. */
  void forgetSession();

  /** @returns A header carrying the token of the session, 0 before it, and
   * the next sequence, which it uses up. */
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
  std::uint64_t sessionToken_{0};
  std::uint16_t nextSequence_{0};
  std::uint16_t newestReceivedSequence_{0};
  std::uint32_t nextInputNumber_{network::FIRST_INPUT_NUMBER};
  engine::Duration sinceRequest_{};
  engine::Duration sinceConnect_{};
  engine::Duration sinceReadyState_{};
  ConnectionStatus status_;
  std::vector<network::LobbyPlayer> lobbyPlayers_;
  std::optional<bool> requestedReady_;
  bool gameStarted_ = false;
  std::optional<Departure> departure_;
  std::vector<network::IncomingDatagram> received_;
};

}  // namespace rtype::client
