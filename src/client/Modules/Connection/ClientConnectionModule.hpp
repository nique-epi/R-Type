#pragma once

#include <string>
#include <vector>
#include "DatagramRouter.hpp"
#include "EntityMessageQueue.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "MessageDispatcher.hpp"
#include "NetworkThread.hpp"
#include "ServerSession.hpp"
#include "WaitingRoom.hpp"

namespace rtype::client {

/**
 * @brief The module of the connection to a server: the UDP socket on its
 * NetworkThread, a DatagramRouter in front of it, and the three parts the
 * screens and the game use, which it provides to the modules installed after
 * it: the ServerSession (the attempt and the session), the WaitingRoom and the
 * EntityMessageQueue.
 *
 * install() provides the three parts, over a socket still closed, and adds to
 * NETWORK_STAGE the work that reads what the server sent and sends what is
 * due. start() opens the socket on every IPv4 interface, on a port the
 * system picks, and starts the network thread. stop() leaves the session with
 * the server, if any: it returns once every copy of the Disconnection went
 * out, DISCONNECTION_COPY_INTERVAL apart, so the server frees the place of the
 * player at once; then it closes the socket. A failure while leaving is
 * logged, never thrown.
 */
class ClientConnectionModule final : public engine::IModule {
 public:
  ClientConnectionModule();

  /** @returns CONNECTION_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns LOGGER_MODULE_NAME: the connection logs from its first frame
   * on. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(engine::InstallContext& context) override;
  /** @throws SocketOpenException when the system refuses to open the
   * socket. */
  void start() override;
  void stop() override;

 private:
  /** @brief Leaves the session with the server, if any, and waits until the
   * connection sent every copy of the Disconnection: after each wait of
   * DISCONNECTION_COPY_INTERVAL, the connection moves by that interval and
   * sends the next copy. */
  void leaveServer();

  NetworkThread network_;
  DatagramRouter router_;
  network::MessageDispatcher dispatcher_;
  WaitingRoom waitingRoom_;
  EntityMessageQueue entityMessages_;
  ServerSession session_;
  logging::Logger logger_;
};

}  // namespace rtype::client
