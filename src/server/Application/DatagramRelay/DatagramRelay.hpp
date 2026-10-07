#pragma once

#include <vector>
#include "BoundedQueue.hpp"
#include "IDatagramSocket.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::server {

/**
 * @brief Moves datagrams between a socket and the two queues the simulation
 * thread drains and fills.
 *
 * Every call must run on the thread that runs the socket's NetworkContext, or
 * before that thread starts. The socket and the queues must outlive the relay.
 */
class DatagramRelay {
 public:
  DatagramRelay(network::IDatagramSocket& socket,
                engine::BoundedQueue<network::IncomingDatagram>& incoming,
                engine::BoundedQueue<network::OutgoingDatagram>& outgoing);

  /**
   * @brief From now on, copies every datagram the socket receives, with its
   * sender, into the incoming queue.
   */
  void startReceiving();

  /**
   * @brief Sends every datagram waiting in the outgoing queue, oldest first.
   * @throws DatagramTooLargeException or InvalidAddressException, from the
   * socket, for a datagram it cannot send; the ones after it are not sent.
   */
  void sendWaiting();

 private:
  network::IDatagramSocket* socket_;
  engine::BoundedQueue<network::IncomingDatagram>* incoming_;
  engine::BoundedQueue<network::OutgoingDatagram>* outgoing_;
  std::vector<network::OutgoingDatagram> sending_;
};

}  // namespace rtype::server
