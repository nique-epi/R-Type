#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramQueues.hpp"
#include "DatagramRelay.hpp"
#include "DatagramSorter.hpp"
#include "Endpoint.hpp"
#include "IDatagramSender.hpp"
#include "IDatagramSocket.hpp"
#include "IPlayerAdmission.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConditions.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"
#include "RejectionLedger.hpp"
#include "RejectionReporter.hpp"
#include "SystemClock.hpp"
#include "TickHandler.hpp"
#include "TrafficCounters.hpp"

namespace rtype::server {

/**
 * @brief The server's two threads: the network thread receives, sorts and
 * sends the datagrams, the simulation thread runs a tick at a fixed rate. They
 * only exchange datagrams through BoundedQueue, so the simulation never waits
 * for a client.
 *
 * The network thread sorts each datagram as it arrives (see DatagramSorter):
 * the datagrams of admitted players go to one queue, connection requests to a
 * smaller one, and every other datagram is dropped there, as is a datagram from
 * the address of a player that does not carry its session token, or from a
 * sender over its rate limit. So a flood of requests, of datagrams from
 * senders that were not admitted or that forge the address of a player, or of
 * a player with its own token, cannot push a player's datagram out of its
 * queue.
 * Each tick, the simulation thread drains both queues, hands what they held
 * to the tick handler, then asks the network thread to send what was queued.
 * The DatagramRelay only sends. Every TICKS_PER_REPORT ticks, the simulation
 * thread writes the rejection report at the warn level (see RejectionReporter),
 * from the RejectionLedger the sorter feeds on the network thread.
 *
 * Building the object binds nothing: the socket is bound by open(), so a part
 * of a program that builds it while it installs keeps no resource open until
 * it starts.
 */
class ServerApplication final : public IDatagramSender,
                                public IPlayerAdmission {
 public:
  /**
   * @brief Prepares a server that will listen on @p localEndpoint once open()
   * is called; port 0 lets the system pick a free port. Binds nothing.
   * @param handleTick Must not be empty.
   * @param conditions The network its datagrams cross, see
   * network::openDatagramSocket(); a perfect network unless given.
   */
  ServerApplication(network::Endpoint localEndpoint, TickHandler handleTick,
                    const network::NetworkConditions& conditions = {});

  ServerApplication(const ServerApplication&) = delete;
  ServerApplication& operator=(const ServerApplication&) = delete;
  ServerApplication(ServerApplication&&) = delete;
  ServerApplication& operator=(ServerApplication&&) = delete;
  ~ServerApplication() override = default;

  /**
   * @brief Changes the endpoint open() binds, from the one the constructor
   * was given. Binds nothing.
   * @throws ServerAlreadyOpenException when the socket is already bound.
   */
  void listenOn(network::Endpoint localEndpoint);

  /**
   * @brief Binds the UDP socket on the endpoint given to the constructor, or
   * to listenOn() when it was called.
   * @throws ServerAlreadyOpenException when the socket is already bound.
   * @throws InvalidAddressException when the address is not an IP address.
   * @returns The address and port the socket is bound to: on port 0, the port
   * the system picked.
   * @throws SocketOpenException when the system refuses the port.
   */
  network::Endpoint open();

  /**
   * @brief Unbinds the socket, so the port is free again. Call it once run()
   * returned; does nothing when the server is not open. Open it again with
   * open().
   */
  void close();

  /**
   * @brief Runs the network on the calling thread and the ticks on a thread of
   * their own, until stop() is called or either thread fails, then waits for
   * both. The first tick is due one tick after the call. Call it once, after
   * open().
   * @throws ServerNotOpenException when open() was not called.
   * @throws The error that stopped either thread.
   */
  void run();

  /**
   * @brief Makes run() return, from any thread: the network thread stops at
   * once, the simulation thread when its current sleep ends.
   */
  void stop();

  /** @brief Queues a datagram for the network thread to send at the end of the
   * current tick, as IDatagramSender says. The tick handler runs on the
   * simulation thread. */
  void queueDatagram(network::OutgoingDatagram datagram) override;

  /** @brief Admits @p player, as IPlayerAdmission says; the players reach the
   * tick handler in ReceivedDatagrams::players. */
  void admit(const network::Endpoint& player,
             std::uint64_t sessionToken) override;

  /** @brief Forgets @p player, as IPlayerAdmission says; a datagram it sent
   * before the call took effect still reaches the tick handler. */
  void forget(const network::Endpoint& player,
              std::uint64_t sessionToken) override;

  /**
   * @returns What the socket of the server sent and received, by player and by
   * type of message, since the object was built. Safe from any thread; it lives
   * as long as this object, across close() and open().
   */
  [[nodiscard]] network::TrafficCounters& traffic();

 private:
  void simulateTick();

  engine::SystemClock clock_;
  network::Endpoint listeningEndpoint_;
  network::NetworkConditions conditions_;
  network::TrafficCounters traffic_;
  network::NetworkContext network_;
  std::unique_ptr<network::IDatagramSocket> socket_;
  concurrency::BoundedQueue<network::IncomingDatagram> incoming_;
  concurrency::BoundedQueue<network::IncomingDatagram> connectionRequests_;
  concurrency::BoundedQueue<network::OutgoingDatagram> outgoing_;
  DatagramQueues destinations_;
  RejectionLedger rejections_;
  DatagramSorter sorter_;
  RejectionReporter rejectionReporter_;
  std::optional<network::DatagramRelay> relay_;
  TickHandler handleTick_;
  std::vector<network::IncomingDatagram> received_;
  std::vector<network::IncomingDatagram> receivedRequests_;
};

}  // namespace rtype::server
