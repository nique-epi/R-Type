#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where an arrow moves the cursor over the cloth, as the player sees
 * the cloth on screen. */
enum class ClothDirection : std::uint8_t {
  Left,
  Right,
  Up,
  Down,
};

}  // namespace rtype::client
