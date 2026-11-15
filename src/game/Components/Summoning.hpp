#pragma once

#include <cstddef>

namespace rtype::game {

/**
 * @brief Component of an enemy that does not take part yet, for the ticks of
 * the game left before it does: an enemy a volley just summoned, while the
 * client shows the warning of its arrival, or a boss during its warning.
 * Until `ticksLeft` reaches 0 it stands still, fires nothing, hurts no ship
 * and takes no hit. One less at each tick (see VolleySystem).
 */
struct Summoning {
  std::size_t ticksLeft{0};
};

}  // namespace rtype::game
