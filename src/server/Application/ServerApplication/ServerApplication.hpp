#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramRelay.hpp"
#include "DatagramSorter.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "Logger.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"
#include "SystemClock.hpp"
#include "UdpSocket.hpp"

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
 */
class ServerApplication {
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
   * @brief Queues a datagram for the network thread to send at the end of the
   * current tick. Call it from the tick handler, which runs on the simulation
   * thread. When more than OUTGOING_DATAGRAM_QUEUE_CAPACITY wait, the oldest
   * is discarded.
   */
  void queueDatagram(network::OutgoingDatagram datagram);

  /**
   * @brief Sends, on the calling thread, every datagram queued and not sent
   * yet, oldest first, so a program can still tell its players it stops
   * once run() returned. Call it only after run() returned: no other thread
   * may run the network then.
   *
   * The system takes each datagram within the call: Asio 1.32 tries the
   * send at once on the kqueue and epoll reactors and calls WSASendTo on
   * Windows, so nothing has to run afterwards. A datagram the system cannot
   * take at once, because the send buffer of the socket is full, is lost.
   * @throws DatagramTooLargeException or InvalidAddressException, from the
   * socket, for a datagram it cannot send; the ones after it are not sent.
   */
  void sendQueuedAfterRun();

  /**
   * @brief From now on, the datagrams @p player sends with @p sessionToken
   * reach the tick handler in ReceivedDatagrams::players, and those it sends
   * with another token are dropped. Call it from any thread, once the player
   * holds a place; it takes effect on the network thread shortly after, and a
   * datagram it receives before that is dropped like a stranger's.
   */
  void admit(const network::Endpoint& player, std::uint64_t sessionToken);

  /**
   * @brief From now on, the datagrams @p player sends are dropped like a
   * stranger's, unless it was admitted again with another token than
   * @p sessionToken. Call it from any thread once the session of the player
   * closed; it takes effect on the network thread shortly after, so a datagram
   * it receives before that still reaches the tick handler.
   */
  void forget(const network::Endpoint& player, std::uint64_t sessionToken);

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
  concurrency::BoundedQueue<network::IncomingDatagram> incoming_;
  concurrency::BoundedQueue<network::IncomingDatagram> connectionRequests_;
  concurrency::BoundedQueue<network::OutgoingDatagram> outgoing_;
  DatagramSorter sorter_;
  network::DatagramRelay relay_;
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
