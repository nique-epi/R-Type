#pragma once

#include <cstddef>
#include <functional>
#include <span>
#include "Endpoint.hpp"

namespace rtype::network {

/**
 * @brief Called once per datagram received, with its sender and its bytes.
 *
 * The bytes are only valid during the call: copy them to keep them.
 */
using DatagramHandler = std::function<void(const Endpoint& sender,
                                           std::span<const std::byte> payload)>;

/**
 * @brief Sends and receives UDP datagrams without blocking.
 *
 * Nothing in it names Asio, so the code that uses a socket never includes it.
 * A socket is not thread-safe: use it, and destroy it, on the thread that runs
 * its NetworkContext, or before that thread starts running it.
 */
class IDatagramSocket {
 public:
  virtual ~IDatagramSocket() = default;

  /**
   * @brief Hands every datagram received from now on to @p handler.
   *
   * A datagram larger than MAX_DATAGRAM_SIZE never reaches the handler: it is
   * dropped. An error while receiving is logged and the socket keeps
   * receiving. The handler must not throw: an exception leaves
   * NetworkContext::run() and the socket stops receiving.
   *
   * @throws ReceivingAlreadyStartedException when the socket already hands
   *         its datagrams to a handler.
   */
  virtual void startReceiving(DatagramHandler handler) = 0;

  /**
   * @brief Copies @p payload and queues it for @p destination, then returns
   *        at once. A datagram the system fails to send is logged and lost,
   *        like any UDP datagram.
   *
   * @throws DatagramTooLargeException when @p payload holds more than
   *         MAX_DATAGRAM_SIZE bytes. Nothing is sent.
   * @throws InvalidAddressException when the address of @p destination is not
   *         an IP address. Nothing is sent.
   */
  virtual void send(const Endpoint& destination,
                    std::span<const std::byte> payload) = 0;

  /**
   * @return the address and port the socket is bound to: when it was opened
   *         on port 0, the port the system picked.
   */
  [[nodiscard]] virtual Endpoint localEndpoint() const = 0;
};

}  // namespace rtype::network
