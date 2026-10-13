#pragma once

#include <vector>
#include "BoundedQueue.hpp"
#include "IDatagramSocket.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::network {

/**
 * @brief Moves datagrams between a socket and the two queues another thread
 * drains and fills, such as the simulation thread of the server.
 *
 * Every call must run on the thread that runs the socket's NetworkContext, or
 * before that thread starts. The socket and the outgoing queue must outlive the
 * relay. The incoming queue must live longer still: startReceiving() gives the
 * socket a handler that writes to it, and the socket keeps that handler until
 * it is destroyed. Keep the incoming queue alive until the socket is
 * destroyed, or until no thread will run the socket's NetworkContext again.
 */
class DatagramRelay {
 public:
  DatagramRelay(IDatagramSocket& socket,
                concurrency::BoundedQueue<IncomingDatagram>& incoming,
                concurrency::BoundedQueue<OutgoingDatagram>& outgoing);

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
  IDatagramSocket* socket_;
  concurrency::BoundedQueue<IncomingDatagram>* incoming_;
  concurrency::BoundedQueue<OutgoingDatagram>* outgoing_;
  std::vector<OutgoingDatagram> sending_;
};

}  // namespace rtype::network
