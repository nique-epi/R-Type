#pragma once

#include <exception>
#include <mutex>
#include <thread>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramRelay.hpp"
#include "Endpoint.hpp"
#include "IDatagramLink.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"
#include "UdpSocket.hpp"

namespace rtype::client {

/**
 * @brief The client's UDP socket, run on a thread of its own so the frame
 * loop never waits on the network.
 *
 * The network thread runs the socket's NetworkContext and is the only thread
 * that touches the socket. A DatagramRelay, the same as the server's, copies
 * every datagram received into the incoming queue the frame loop drains, and
 * sends what the frame loop left in the outgoing queue. Every public call is
 * safe from any thread.
 *
 * An exception on the network thread, such as a datagram it cannot send,
 * stops that thread; drainReceived() then throws it.
 */
class NetworkThread : public IDatagramLink {
 public:
  /**
   * @brief Opens the socket on @p localEndpoint, starts receiving, then starts
   * the network thread. Port 0 lets the system pick a free port.
   * @throws InvalidAddressException when the address is not an IP address.
   * @throws SocketOpenException when the system refuses the port.
   */
  explicit NetworkThread(const network::Endpoint& localEndpoint);

  /**
   * @brief Stops the network thread and waits for it. A datagram handed to
   * send() and not sent yet is lost.
   */
  ~NetworkThread() override;

  NetworkThread(const NetworkThread&) = delete;
  NetworkThread& operator=(const NetworkThread&) = delete;
  NetworkThread(NetworkThread&&) = delete;
  NetworkThread& operator=(NetworkThread&&) = delete;

  /**
   * @brief Puts @p datagram in the outgoing queue and asks the network thread
   * to send what waits there, then returns at once.
   *
   * When more than OUTGOING_DATAGRAM_QUEUE_CAPACITY datagrams wait, the oldest
   * is discarded. A datagram the network thread cannot send stops it: more
   * than MAX_DATAGRAM_SIZE bytes, or a destination that is not an IP address.
   */
  void send(network::OutgoingDatagram datagram) override;

  /**
   * @brief Moves every datagram received since the previous call, oldest
   * first, to the end of @p destination. Returns at once when nothing arrived.
   *
   * When more than INCOMING_DATAGRAM_QUEUE_CAPACITY datagrams arrive between
   * two calls, the oldest are discarded.
   *
   * @throws The error that stopped the network thread, on every call once it
   * stopped; nothing is moved then.
   */
  void drainReceived(
      std::vector<network::IncomingDatagram>& destination) override;

  /**
   * @returns The address and port the socket is bound to: when it was opened
   * on port 0, the port the system picked.
   */
  [[nodiscard]] network::Endpoint localEndpoint() const;

 private:
  /** @brief Runs the network until it is stopped, and keeps the error that
   * stopped it, if any. */
  void runNetwork();

  /** @brief Throws the error that stopped the network thread, if any. */
  void rethrowNetworkError() const;

  network::NetworkContext network_;
  network::UdpSocket socket_;
  network::Endpoint localEndpoint_;
  concurrency::BoundedQueue<network::IncomingDatagram> incoming_;
  concurrency::BoundedQueue<network::OutgoingDatagram> outgoing_;
  network::DatagramRelay relay_;
  mutable std::mutex networkErrorMutex_;
  std::exception_ptr networkError_;
  std::jthread thread_;
};

}  // namespace rtype::client
