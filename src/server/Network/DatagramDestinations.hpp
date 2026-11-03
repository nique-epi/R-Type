#pragma once

#include "BoundedQueue.hpp"
#include "IncomingDatagram.hpp"

namespace rtype::server {

/**
 * @brief The queues a DatagramSorter puts the datagrams it keeps in, named so
 * they cannot be swapped by mistake. Both must outlive the sorter.
 */
struct DatagramDestinations {
  concurrency::BoundedQueue<network::IncomingDatagram>* players{};
  concurrency::BoundedQueue<network::IncomingDatagram>* connectionRequests{};
};

}  // namespace rtype::server
