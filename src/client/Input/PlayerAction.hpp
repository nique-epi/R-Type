#pragma once

#include <cstdint>
#include "ProtocolConstants.hpp"

namespace rtype::client {

/**
 * @brief What the player can ask the ship to do.
 *
 * The value of each action is its bit position in the `actions` byte of the
 * `PlayerInput` message, as the protocol constants name it: 0 up, 1 down, 2
 * left, 3 right, 4 shoot.
 */
enum class PlayerAction : std::uint8_t {
  Up = network::UP_ACTION_BIT_POSITION,
  Down = network::DOWN_ACTION_BIT_POSITION,
  Left = network::LEFT_ACTION_BIT_POSITION,
  Right = network::RIGHT_ACTION_BIT_POSITION,
  Shoot = network::SHOOT_ACTION_BIT_POSITION,
};

}  // namespace rtype::client
