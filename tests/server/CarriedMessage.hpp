#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rtype::server::testing {

/**
 * @brief A message a datagram carries as a client reads it: its type byte and
 * a copy of its payload, taken out of the OrderedMessage that carried it when
 * it was sent on the ordered channel.
 */
struct CarriedMessage {
  std::uint8_t type;
  std::vector<std::byte> payload;
};

}  // namespace rtype::server::testing
