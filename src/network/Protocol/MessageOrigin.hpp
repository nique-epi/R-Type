#pragma once

#include "Endpoint.hpp"
#include "PacketHeader.hpp"

namespace rtype::network {

/**
 * @brief Where a received message comes from: the address and port its
 * datagram was sent from, and the header of that datagram, which carries the
 * session token and the sequence.
 */
struct MessageOrigin {
  Endpoint sender;
  PacketHeader header;
};

}  // namespace rtype::network
