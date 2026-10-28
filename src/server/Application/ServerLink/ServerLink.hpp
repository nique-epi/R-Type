#pragma once

#include <cstdint>
#include <optional>
#include "Endpoint.hpp"
#include "IServerLink.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerApplication.hpp"

namespace rtype::server {

/**
 * @brief The IServerLink of the network module: the ServerApplication it opens
 * in its start() and closes in its stop(), reached by the other modules from
 * their install() on.
 *
 * The other modules only use it while the loop runs, or from their stop(),
 * which the Application calls before the stop() of the network module: the
 * link is open then. A call while it is closed is a defect of the program and
 * throws ServerLinkClosedException.
 */
class ServerLink : public IServerLink {
 public:
  /**
   * @brief Opens the UDP socket on @p localEndpoint, as the constructor of
   * ServerApplication does; a link already open is closed first.
   * @throws What the constructor of ServerApplication throws.
   */
  void open(const network::Endpoint& localEndpoint,
            ServerApplication::TickHandler handleTick);

  /**
   * @brief Closes the socket; does nothing when the link is closed. Call it
   * once run() returned.
   */
  void close();

  /**
   * @brief Runs the network and the ticks until stop(), as
   * ServerApplication::run() does.
   * @throws ServerLinkClosedException when the link is closed.
   * @throws The error that stopped either thread.
   */
  void run();

  /**
   * @brief Makes run() return, from any thread; does nothing when the link is
   * closed.
   */
  void stop();

  /**
   * @returns The address and port the socket is bound to.
   * @throws ServerLinkClosedException when the link is closed.
   */
  [[nodiscard]] network::Endpoint localEndpoint() const;

  void send(network::OutgoingDatagram datagram) override;
  void admit(const network::Endpoint& player,
             std::uint64_t sessionToken) override;
  void forget(const network::Endpoint& player,
              std::uint64_t sessionToken) override;
  void sendQueuedAfterRun() override;

 private:
  /**
   * @returns The server of the open link.
   * @throws ServerLinkClosedException when the link is closed.
   */
  [[nodiscard]] ServerApplication& openServer();

  std::optional<ServerApplication> server_;
};

}  // namespace rtype::server
