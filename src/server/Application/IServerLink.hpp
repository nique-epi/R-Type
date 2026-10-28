#pragma once

#include <cstdint>
#include "Endpoint.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::server {

/**
 * @brief What the modules of the server send to the players and tell the
 * network thread through: the service the network module provides during its
 * install(), so every module holds it before the socket is open.
 *
 * send(), admit() and forget() are called from the work of a loop stage, on
 * the simulation thread, while the server runs; sendQueuedAfterRun() from the
 * stop() of a module, once the loop stopped.
 */
class IServerLink {
 public:
  virtual ~IServerLink() = default;

  /**
   * @brief Queues @p datagram, sent at the end of the current tick, as
   * ServerApplication::queueDatagram() does.
   */
  virtual void send(network::OutgoingDatagram datagram) = 0;

  /**
   * @brief From now on, the datagrams @p player sends with @p sessionToken
   * reach the games, as ServerApplication::admit() says.
   */
  virtual void admit(const network::Endpoint& player,
                     std::uint64_t sessionToken) = 0;

  /**
   * @brief From now on, the datagrams @p player sends are dropped like a
   * stranger's, as ServerApplication::forget() says.
   */
  virtual void forget(const network::Endpoint& player,
                      std::uint64_t sessionToken) = 0;

  /**
   * @brief Sends at once what send() queued, once the loop stopped and before
   * the network module closes the socket, as
   * ServerApplication::sendQueuedAfterRun() does.
   */
  virtual void sendQueuedAfterRun() = 0;
};

}  // namespace rtype::server
