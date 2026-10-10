#pragma once

#include <cstddef>
#include <vector>
#include "Endpoint.hpp"

namespace rtype::network {

/**
 * @brief A datagram waiting to be sent: where it goes, and its bytes.
 */
struct OutgoingDatagram {
  Endpoint destination;
  std::vector<std::byte> payload;

  bool operator==(const OutgoingDatagram&) const = default;
};

}  // namespace rtype::network
