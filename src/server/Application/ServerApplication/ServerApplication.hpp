#pragma once

#include <cstddef>
#include <functional>
#include <span>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramRelay.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "Logger.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"
#include "SystemClock.hpp"
#include "UdpSocket.hpp"

namespace rtype::server {

/**
 * @brief The server's two threads: the network thread receives and sends the
 * datagrams, the simulation thread runs a tick at a fixed rate. They only
 * exchange datagrams through two BoundedQueue, so the simulation never waits
 * for a client.
 *
 * Each tick, the simulation thread drains every datagram received since the
 * previous tick, hands them to the tick handler, then asks the network thread
 * to send what was queued.
 */
class ServerApplication {
 public:
  /**
   * @brief Called once per tick, on the simulation thread, with the datagrams
   * received since the previous tick, oldest first; often none. An exception
   * it throws stops the server and leaves run().
   */
  using TickHandler =
      std::function<void(std::span<const network::IncomingDatagram>)>;

  /**
   * @brief Opens the UDP socket on @p localEndpoint; port 0 lets the system
   * pick a free port.
   * @param handleTick Must not be empty.
   * @throws InvalidAddressException when the address is not an IP address.
   * @throws SocketOpenException when the system refuses the port.
   */
  ServerApplication(const network::Endpoint& localEndpoint,
                    TickHandler handleTick);

  ServerApplication(const ServerApplication&) = delete;
  ServerApplication& operator=(const ServerApplication&) = delete;
  ServerApplication(ServerApplication&&) = delete;
  ServerApplication& operator=(ServerApplication&&) = delete;
  ~ServerApplication() = default;

  /**
   * @brief Runs the network on the calling thread and the ticks on a thread of
   * their own, until stop() is called or either thread fails, then waits for
   * both. The first tick is due one tick after the call. Call it once.
   * @throws The error that stopped either thread.
   */
  void run();

  /**
   * @brief Makes run() return, from any thread: the network thread stops at
   * once, the simulation thread when its current sleep ends.
   */
  void stop();

  /**
   * @returns The address and port the socket is bound to.
   */
  [[nodiscard]] network::Endpoint localEndpoint() const;

 private:
  void simulateTick();
  void reportDiscardedDatagrams();

  logging::Logger logger_;
  engine::SystemClock clock_;
  network::NetworkContext network_;
  network::UdpSocket socket_;
  engine::BoundedQueue<network::IncomingDatagram> incoming_;
  engine::BoundedQueue<network::OutgoingDatagram> outgoing_;
  DatagramRelay relay_;
  TickHandler handleTick_;
  std::vector<network::IncomingDatagram> received_;
  std::size_t reportedDiscardCount_{0};
};

}  // namespace rtype::server
