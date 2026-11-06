#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief A direction an arrow key moves the choice of a star towards. */
enum class MapDirection : std::uint8_t {
  Up,
  Down,
  Left,
  Right,
};

}  // namespace rtype::client
