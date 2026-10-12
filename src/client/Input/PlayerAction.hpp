#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief What the player can ask the ship to do.
 *
 * The value of each action is its bit position in the `actions` byte of the
 * `PlayerInput` message: 0 up, 1 down, 2 left, 3 right, 4 shoot.
 */
enum class PlayerAction : std::uint8_t {
  Up = 0,
  Down = 1,
  Left = 2,
  Right = 3,
  Shoot = 4,
};

}  // namespace rtype::client
