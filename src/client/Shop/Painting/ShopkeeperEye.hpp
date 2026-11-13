#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How the eye of the shopkeeper shows, in the order of the frames
 * of his sheet: looking ahead, to his left, to his right, its lid half
 * down, and closed. */
enum class ShopkeeperEye : std::uint8_t {
  Ahead,
  Left,
  Right,
  HalfClosed,
  Closed,
};

}  // namespace rtype::client
