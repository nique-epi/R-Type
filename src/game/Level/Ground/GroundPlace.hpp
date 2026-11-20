#pragma once

namespace rtype::game {

/**
 * @brief Where the ground of a biome is looked up: the column `x` of the
 * playfield, in logical units, and how far the level scrolled since the
 * biome began, in logical units.
 */
struct GroundPlace {
  float x{};
  float scrolled{};
};

}  // namespace rtype::game
