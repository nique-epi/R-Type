#pragma once

namespace rtype::client {

/**
 * @brief What a DatagramRoute asks of the router that made it: to hand out
 * the datagrams the socket received, so the route reads what is waiting for
 * it, never an older state of the socket.
 */
class IDatagramPump {
 public:
  virtual ~IDatagramPump() = default;

  /** @brief Moves every datagram the socket received since the previous call
   * to the route that listens to its sender. */
  virtual void pump() = 0;
};

}  // namespace rtype::client
