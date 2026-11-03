#pragma once

#include "OutgoingDatagram.hpp"

namespace rtype::server {

/** @brief Where the parts of the server put the datagrams they send to the
 * players. */
class IDatagramSender {
 public:
  virtual ~IDatagramSender() = default;

  /**
   * @brief Queues @p datagram for the network thread to send at the end of the
   * current tick. When more than OUTGOING_DATAGRAM_QUEUE_CAPACITY wait, the
   * oldest is discarded. Safe from any thread.
   */
  virtual void queueDatagram(network::OutgoingDatagram datagram) = 0;
};

}  // namespace rtype::server
