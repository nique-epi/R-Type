#pragma once

#include <vector>
#include "PastBeam.hpp"
#include "PastEnemy.hpp"

namespace rtype::game {

/** @brief The enemies at one tick: their bodies, which a missile of a player
 * hits and a ship runs into, and the other threats to a ship, the missiles of
 * the enemies and the active boxes of their HurtZones, then the active beams
 * of those HurtZones. */
struct PastTick {
  std::vector<PastEnemy> bodies;
  std::vector<PastEnemy> threats;
  std::vector<PastBeam> beams;
};

}  // namespace rtype::game
