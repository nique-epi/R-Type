#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief The code of one symbol: its @c length lowest bits of @c bits, most
 * significant bit first.
 */
struct HuffmanCode {
  std::uint16_t bits{0};
  std::uint8_t length{0};
};

}  // namespace rtype::network
