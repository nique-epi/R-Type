#pragma once

#include <cstddef>

namespace rtype::game {

/**
 * @brief How many ticks behind the present the player who fired a missile saw
 * the enemies: LagCompensatedHitSystem tests that missile against the enemies
 * as they were that many ticks ago, so it hits what its player saw it hit.
 */
struct ViewDelay {
  std::size_t ticks{};
};

}  // namespace rtype::game
