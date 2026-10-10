#pragma once

#include <cstddef>
#include <vector>
#include "Endpoint.hpp"

namespace rtype::network {

/**
 * @brief A datagram a socket received, copied out of the receive buffer: who
 * sent it, and its bytes.
 */
struct IncomingDatagram {
  Endpoint sender;
  std::vector<std::byte> payload;

  bool operator==(const IncomingDatagram&) const = default;
};

}  // namespace rtype::network
