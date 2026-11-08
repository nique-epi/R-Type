#pragma once

#include <vector>
#include "BeamLine.hpp"
#include "Bounds.hpp"

namespace rtype::game {

/**
 * @brief What of the HurtZone of one enemy hurts at one tick: its active
 * boxes, laid on the enemy, and its active beams, laid from their pivots.
 * layActiveHurts() fills it; HurtZoneSystem tests the ships against it in the
 * present, LagCompensatedHitSystem keeps it to test a ship where its player
 * saw the enemy.
 */
struct ActiveHurts {
  std::vector<engine::Bounds> boxes;
  std::vector<BeamLine> beams;
};

}  // namespace rtype::game
