#pragma once

#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramRelay.hpp"
#include "Endpoint.hpp"
#include "IDatagramLink.hpp"
#include "IDatagramSocket.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConditions.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::client {

/**
 * @brief The client's UDP socket, run on a thread of its own so the frame
 * loop never waits on the network.
 *
 * Building it opens nothing: open() binds the socket and starts the network
 * thread, close() stops the thread and closes the socket, so the module that
 * owns it builds it while it installs and opens it when it starts.
 *
 * The network thread runs the socket's NetworkContext and is the only thread
 * that touches the socket. A DatagramRelay, the same as the server's, copies
 * every datagram received into the incoming queue the frame loop drains, and
 * sends what the frame loop left in the outgoing queue. send() and
 * drainReceived() are safe from any thread.
 *
 * An exception on the network thread, such as a datagram it cannot send,
 * stops that thread; drainReceived() then throws it.
 */
class NetworkThread : public IDatagramLink {
 public:
  /**
   * @brief Keeps @p localEndpoint and @p conditions for open(), and binds
   * nothing. Port 0 lets the system pick a free port when the socket opens.
   * @param conditions The network the datagrams cross, see
   * network::openDatagramSocket().
   */
  NetworkThread(network::Endpoint localEndpoint,
                const network::NetworkConditions& conditions);

  /** @brief Closes the network thread and the socket, as close() does. */
  ~NetworkThread() override;

  NetworkThread(const NetworkThread&) = delete;
  NetworkThread& operator=(const NetworkThread&) = delete;
  NetworkThread(NetworkThread&&) = delete;
  NetworkThread& operator=(NetworkThread&&) = delete;

  /**
   * @brief Opens the socket on the endpoint given to the constructor, starts
   * receiving, then starts the network thread; does nothing when it is
   * already open. A network thread closed by close() is not opened again:
   * its NetworkContext stays stopped.
   * @throws InvalidAddressException when the address is not an IP address.
   * @throws SocketOpenException when the system refuses the port.
   */
  void open();

  /**
   * @brief Has the network thread send what waits in the outgoing queue, then
   * stop, waits for it, and closes the socket, so a datagram handed to send()
   * before is not lost and the port is free again. Does nothing when it was
   * never opened. When an error already stopped that thread, what waits is
   * lost.
   */
  void close();

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
   * on port 0, the port the system picked. Call it once open() returned.
   */
  [[nodiscard]] network::Endpoint localEndpoint() const;

 private:
  /** @brief Starts receiving on @p socket, just opened, then starts the
   * network thread. */
  void startNetwork(network::IDatagramSocket& socket);

  /** @brief Runs the network until it is stopped, and keeps the error that
   * stopped it, if any. */
  void runNetwork();

  /** @brief Throws the error that stopped the network thread, if any. */
  void rethrowNetworkError() const;

  network::Endpoint listeningEndpoint_;
  network::NetworkConditions conditions_;
  network::NetworkContext network_;
  std::unique_ptr<network::IDatagramSocket> socket_;
  network::Endpoint localEndpoint_;
  concurrency::BoundedQueue<network::IncomingDatagram> incoming_;
  concurrency::BoundedQueue<network::OutgoingDatagram> outgoing_;
  std::optional<network::DatagramRelay> relay_;
  mutable std::mutex networkErrorMutex_;
  std::exception_ptr networkError_;
  std::jthread thread_;
};

}  // namespace rtype::client
