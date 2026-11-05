#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::tools {

/**
 * @brief Who a flood goes to: the port of the program on the loopback
 * address, the session token its datagrams carry, and how many it sends per
 * second.
 */
struct FloodRecipient {
  std::uint16_t port{};
  std::uint64_t sessionToken{};
  std::size_t datagramsPerSecond{};
};

}  // namespace rtype::tools
