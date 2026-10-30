#pragma once

#include <deque>
#include <vector>
#include "DatagramRoute.hpp"
#include "IDatagramLink.hpp"
#include "IDatagramPump.hpp"
#include "IncomingDatagram.hpp"

namespace rtype::client {

/**
 * @brief Stands in front of the client's socket so several recipients read it
 * together: each one owns a DatagramRoute that listens to one sender, and
 * receives the datagrams of that sender and no others.
 *
 * Before, the connection to the game server was the only reader of the
 * socket and dropped whatever the game server did not send. With a route per
 * recipient, a client of another server reads its answers while a game is
 * open.
 *
 * pump() empties the socket once and hands each datagram to the route that
 * listens to its sender; a datagram nobody listens to is dropped. A route
 * pumps before it reads, so no recipient has to call pump() itself. Not
 * thread-safe: use it on the thread of the frame loop.
 */
class DatagramRouter final : public IDatagramPump {
 public:
  /** @param socket The socket every route reads and sends through; it must
   * outlive the router. */
  explicit DatagramRouter(IDatagramLink& socket);

  ~DatagramRouter() override = default;
  DatagramRouter(const DatagramRouter&) = delete;
  DatagramRouter& operator=(const DatagramRouter&) = delete;
  DatagramRouter(DatagramRouter&&) = delete;
  DatagramRouter& operator=(DatagramRouter&&) = delete;

  /** @returns A new route, which listens to nobody until listenTo(); it stays
   * valid as long as the router lives. */
  [[nodiscard]] DatagramRoute& addRoute();

  /**
   * @brief Hands each datagram the socket received since the previous call to
   * the route that listens to its sender, or drops it.
   * @throws What the socket throws when it drains.
   */
  void pump() override;

 private:
  IDatagramLink* socket_;
  std::deque<DatagramRoute> routes_;
  std::vector<network::IncomingDatagram> drained_;
};

}  // namespace rtype::client
