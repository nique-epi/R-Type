#pragma once

#include <vector>
#include "PastEnemy.hpp"

namespace rtype::game {

/** @brief The boxes of the enemies at one tick: their bodies, which a missile
 * of a player hits and a ship runs into, and the other threats to a ship,
 * the missiles of the enemies and the active boxes of their HurtZones. */
struct PastTick {
  std::vector<PastEnemy> bodies;
  std::vector<PastEnemy> threats;
};

}  // namespace rtype::game
