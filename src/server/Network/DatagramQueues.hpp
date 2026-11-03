#pragma once

#include "BoundedQueue.hpp"
#include "IncomingDatagram.hpp"

namespace rtype::server {

/**
 * @brief The queues the DatagramSorter hands the datagrams to, named so they
 * cannot be swapped by mistake. Both must outlive the sorter.
 */
struct DatagramQueues {
  concurrency::BoundedQueue<network::IncomingDatagram>* players{};
  concurrency::BoundedQueue<network::IncomingDatagram>* connectionRequests{};
};

}  // namespace rtype::server
