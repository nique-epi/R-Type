#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramRelay.hpp"
#include "DatagramSorter.hpp"
#include "Endpoint.hpp"
#include "IDatagramSender.hpp"
#include "IDatagramSocket.hpp"
#include "IPlayerAdmission.hpp"
#include "IncomingDatagram.hpp"
#include "Logger.hpp"
#include "NetworkConditions.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"
#include "SystemClock.hpp"

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
 * the address of a player that does not carry its session token. So a flood
 * of requests, or of datagrams from senders that were not admitted or that
 * forge the address of a player, cannot push a player's datagram out of its
 * queue; a player flooding with its own token still can.
 * Each tick, the simulation thread drains both queues, hands what they held
 * to the tick handler, then asks the network thread to send what was queued.
 * The DatagramRelay only sends.
 *
 * Building the object binds nothing: the socket is bound by open(), so a part
 * of a program that builds it while it installs keeps no resource open until
 * it starts.
 */
class ServerApplication final : public IDatagramSender,
                                public IPlayerAdmission {
 public:
  /**
   * @brief What arrived since the previous tick, oldest first in each list;
   * often nothing.
   */
  struct ReceivedDatagrams {
    /** @brief The datagrams of the players admitted with admit(), each
     * carrying the session token of its player. */
    std::span<const network::IncomingDatagram> players;
    /** @brief The datagrams holding a ConnectionRequest, from strangers or
     * from players with their token or none: at most
     * CONNECTION_REQUEST_QUEUE_CAPACITY. */
    std::span<const network::IncomingDatagram> connectionRequests;
  };

  /**
   * @brief Called once per tick, on the simulation thread, with the datagrams
   * received since the previous tick. An exception it throws stops the server
   * and leaves run().
   *
   * Anyone can send a datagram, so the handler must drop and count a datagram
   * it cannot decode, and never let an exception caused by a datagram's
   * content escape: one truncated datagram would otherwise stop the server
   * for every player.
   */
  using TickHandler = std::function<void(const ReceivedDatagrams&)>;

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
   * @brief Binds the UDP socket on the endpoint given to the constructor.
   * @throws ServerAlreadyOpenException when the socket is already bound.
   * @throws InvalidAddressException when the address is not an IP address.
   * @throws SocketOpenException when the system refuses the port.
   */
  void open();

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
   * @returns The address and port the socket is bound to.
   * @throws ServerNotOpenException when open() was not called.
   */
  [[nodiscard]] network::Endpoint localEndpoint() const;

 private:
  /** @throws ServerNotOpenException when open() was not called. */
  [[nodiscard]] network::IDatagramSocket& openSocket();
  [[nodiscard]] const network::IDatagramSocket& openSocket() const;
  void simulateTick();
  void reportDiscardedDatagrams();

  logging::Logger logger_;
  engine::SystemClock clock_;
  network::Endpoint listeningEndpoint_;
  network::NetworkConditions conditions_;
  network::NetworkContext network_;
  std::unique_ptr<network::IDatagramSocket> socket_;
  concurrency::BoundedQueue<network::IncomingDatagram> incoming_;
  concurrency::BoundedQueue<network::IncomingDatagram> connectionRequests_;
  concurrency::BoundedQueue<network::OutgoingDatagram> outgoing_;
  DatagramSorter sorter_;
  std::optional<network::DatagramRelay> relay_;
  TickHandler handleTick_;
  std::vector<network::IncomingDatagram> received_;
  std::vector<network::IncomingDatagram> receivedRequests_;
  std::size_t ticksSinceDiscardReport_{0};
  std::size_t reportedDiscardCount_{0};
  std::size_t reportedRequestDiscardCount_{0};
  std::size_t reportedDroppedCount_{0};
  std::size_t reportedWrongTokenCount_{0};
};

}  // namespace rtype::server
