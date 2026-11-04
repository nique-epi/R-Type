#pragma once

#include <chrono>
#include "Endpoint.hpp"
#include "PacketHeader.hpp"

namespace rtype::network {

/**
 * @brief Where a received message comes from: the address and port its
 * datagram was sent from, the header of that datagram, which carries the
 * session token and the sequence, and when that datagram arrived.
 */
struct MessageOrigin {
  Endpoint sender;
  PacketHeader header;
  /** @brief IncomingDatagram::receivedAt of the datagram. */
  std::chrono::nanoseconds receivedAt{};
};

}  // namespace rtype::network
