#pragma once

#include <cstddef>
#include <functional>
#include <span>
#include "Endpoint.hpp"
#include "PacketHeader.hpp"
#include "SortedQueue.hpp"

namespace rtype::server {

/**
 * @brief A datagram of this protocol and version whose messages add up, as the
 * DatagramSorter reads it before deciding where it goes. Its sender and its
 * bytes are views of what the network thread received: it lives no longer
 * than the call to DatagramSorter::sort().
 */
struct SpokenDatagram {
  std::reference_wrapper<const network::Endpoint> sender;
  network::PacketHeader header;
  /** @brief The queue it goes to: ConnectionRequests when it holds a
   * ConnectionRequest, whatever else it holds, InformationRequests when it
   * holds a ServerInformationRequest, Players otherwise. */
  SortedQueue queue{SortedQueue::Players};
  std::span<const std::byte> bytes;
};

}  // namespace rtype::server
