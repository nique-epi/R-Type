#pragma once

#include <cstdint>
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
#include "Logger.hpp"
#include "MessageDispatcher.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The attempt of the client to join a server, as the Connection section
 * of the protocol page describes, then the inputs of the game it sends there.
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
 * The session token in the header of the ConnectionAccepted goes in every
 * datagram sent after it. While the attempt is Accepted, sendPlayerInput()
 * sends the inputs of the game, numbered from FIRST_INPUT_NUMBER; otherwise it
 * sends nothing. Requests and inputs take their sequence from the same counter,
 * which starts at 0. connect() starts a new session: sequence 0, token 0 and
 * inputs from FIRST_INPUT_NUMBER again.
 *
 * Only datagrams from the server being contacted, of this protocol and version
 * and whose messages add up to their end, are read; any other is ignored.
 *
 * update() reads what arrived and sends the requests that are due, once per
 * frame; sendPlayerInput() sends at once. Call both on the thread of the frame
 * loop.
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

  /**
   * @brief Reads the datagrams received since the previous call, then sends
   * the request again when it is due, or gives up.
   *
   * @param elapsed Time since the previous call, or since connect().
   */
  void update(engine::Duration elapsed);

  void sendPlayerInput(ActionSet held) override;

 private:
  void registerAnswers();
  void receive(const network::IncomingDatagram& datagram);
  void sendRequest();

  /** @returns The header of the next datagram to the server: the session token,
   * 0 before an acceptance, and the next sequence, which it uses up. */
  [[nodiscard]] network::PacketHeader nextHeader();

  [[nodiscard]] bool isConnecting() const;

  IDatagramLink* link_;
  network::MessageDispatcher dispatcher_;
  logging::Logger logger_;
  network::Endpoint server_;
  std::string nickname_;
  std::uint64_t cookie_{0};
  std::uint64_t sessionToken_{0};
  std::uint16_t nextSequence_{0};
  std::uint32_t nextInputNumber_{network::FIRST_INPUT_NUMBER};
  engine::Duration sinceRequest_{};
  engine::Duration sinceConnect_{};
  ConnectionStatus status_;
  std::vector<network::IncomingDatagram> received_;
};

}  // namespace rtype::client
