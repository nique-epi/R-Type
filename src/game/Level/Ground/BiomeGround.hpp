#pragma once

#include <vector>

namespace rtype::game {

/**
 * @brief The ground of the play plane of a biome, as its LDtk project gives
 * it: for each pixel column of its maps laid end to end, in the order of the
 * file, the row of the top of its floor and the row just under its ceiling,
 * in pixels from the top of the map, GROUND_NONE where the column has none.
 * Both lists are as long as the maps are wide together.
 */
struct BiomeGround {
  std::vector<int> floorRows;
  std::vector<int> ceilingRows;
};

}  // namespace rtype::game
