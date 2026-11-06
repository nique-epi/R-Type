#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What the glyph of a server shows of it: how many pilots it holds,
 * whether it is slow, and whether it speaks another version. */
struct StarLook {
  std::uint16_t playerCount{};
  bool slow{};
  bool incompatible{};
};

}  // namespace rtype::client
