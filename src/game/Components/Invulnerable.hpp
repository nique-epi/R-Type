#pragma once

#include <cstddef>

namespace rtype::game {

/**
 * @brief Component of a player's ship: the ticks of the game left during which
 * the ship cannot be hit. One less at each tick; 0 when it can be hit.
 */
struct Invulnerable {
  std::size_t ticksLeft{0};
};

}  // namespace rtype::game
