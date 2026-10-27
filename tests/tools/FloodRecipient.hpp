#pragma once

#include <cstdint>

namespace rtype::tools {

/**
 * @brief Who a flood goes to: the port of the program on the loopback
 * address, and the session token its datagrams carry.
 */
struct FloodRecipient {
  std::uint16_t port{};
  std::uint64_t sessionToken{};
};

}  // namespace rtype::tools
