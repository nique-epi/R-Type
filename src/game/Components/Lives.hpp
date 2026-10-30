#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Component of a player's ship: the lives the player has left, the ship
 * in play included. Hit with none protecting it, the ship is destroyed and the
 * player gets a new ship with one life less.
 */
struct Lives {
  std::uint8_t count{0};
};

}  // namespace rtype::game
