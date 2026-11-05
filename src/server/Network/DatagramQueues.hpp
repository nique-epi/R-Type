#pragma once

#include "BoundedQueue.hpp"
#include "IncomingDatagram.hpp"

namespace rtype::server {

/**
 * @brief The queues the DatagramSorter hands the datagrams to, named so they
 * cannot be swapped by mistake. All three must outlive the sorter.
 */
struct DatagramQueues {
  concurrency::BoundedQueue<network::IncomingDatagram>* players{};
  concurrency::BoundedQueue<network::IncomingDatagram>* connectionRequests{};
  concurrency::BoundedQueue<network::IncomingDatagram>* informationRequests{};
};

}  // namespace rtype::server
