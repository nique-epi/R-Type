#pragma once

#include "BoundedQueue.hpp"
#include "IncomingDatagram.hpp"

namespace rtype::server {

/**
 * @brief The queues the DatagramSorter hands the datagrams to, named so they
 * cannot be swapped by mistake. All four must outlive the sorter.
 */
struct DatagramQueues {
  concurrency::BoundedQueue<network::IncomingDatagram>* players{};
  concurrency::BoundedQueue<network::IncomingDatagram>* connectionRequests{};
  concurrency::BoundedQueue<network::IncomingDatagram>* informationRequests{};
  concurrency::BoundedQueue<network::IncomingDatagram>* probes{};
};

}  // namespace rtype::server
