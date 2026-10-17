#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "IDatagramLink.hpp"
#include "IServerConnection.hpp"
#include "IncomingDatagram.hpp"
#include "Logger.hpp"
#include "MessageDispatcher.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The attempt of the client to join a server, as the Connection section
 * of the protocol page describes.
 *
 * connect() sends a ConnectionRequest without a cookie at once; while the
 * server does not answer, the request goes out again every
 * CONNECTION_REQUEST_INTERVAL. A ConnectionChallenge is answered at once with
 * the request carrying its cookie, and every later request carries it too. A
 * ConnectionAccepted or a ConnectionRefused ends the attempt, and nothing more
 * is sent. An attempt that got neither within CONNECTION_ATTEMPT_TIMEOUT of
 * connect() gives up as Unreachable: a challenge does not restart that delay.
 *
 * Only datagrams from the server being contacted, of this protocol and version
 * and whose messages add up to their end, are read; any other is ignored.
 *
 * update() reads what arrived and sends what is due: call it once per frame,
 * on the thread of the frame loop.
 */
class ServerConnection : public IServerConnection {
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

 private:
  void registerAnswers();
  void receive(const network::IncomingDatagram& datagram);
  void sendRequest();
  [[nodiscard]] bool isConnecting() const;

  IDatagramLink* link_;
  network::MessageDispatcher dispatcher_;
  logging::Logger logger_;
  network::Endpoint server_;
  std::string nickname_;
  std::uint64_t cookie_{0};
  std::uint16_t nextSequence_{0};
  engine::Duration sinceRequest_{};
  engine::Duration sinceConnect_{};
  ConnectionStatus status_;
  std::vector<network::IncomingDatagram> received_;
};

}  // namespace rtype::client
