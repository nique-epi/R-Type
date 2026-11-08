#pragma once

#include <cstddef>

namespace rtype::game {

/**
 * @brief How many ticks behind the present the player of a missile or a ship
 * saw the enemies: LagCompensatedHitSystem tests that missile or that ship
 * against the enemies as they were that many ticks ago, so a missile hits what
 * its player saw it hit, and a ship is hit by what its player saw hit it.
 */
struct ViewDelay {
  std::size_t ticks{};
};

}  // namespace rtype::game
