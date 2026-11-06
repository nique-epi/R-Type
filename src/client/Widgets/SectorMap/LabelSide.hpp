#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where the name of a star is written: on its right, on its left, or
 * nowhere when neither side is free. */
enum class LabelSide : std::uint8_t {
  Right,
  Left,
  None,
};

}  // namespace rtype::client
