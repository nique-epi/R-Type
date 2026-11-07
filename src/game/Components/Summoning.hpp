#pragma once

#include <cstddef>

namespace rtype::game {

/**
 * @brief Component of an enemy a volley just summoned, for the ticks of the
 * game left before it takes part: until `ticksLeft` reaches 0 it stands still,
 * fires nothing, hurts no ship and takes no hit, while the client shows the
 * warning of its arrival. One less at each tick (see VolleySystem).
 */
struct Summoning {
  std::size_t ticksLeft{0};
};

}  // namespace rtype::game
