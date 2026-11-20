#pragma once

#include <array>
#include <vector>
#include "BulbChase.hpp"
#include "CasinoPaintingConstants.hpp"
#include "ColoredPixel.hpp"
#include "EntranceLightConstants.hpp"
#include "TwinkleGroup.hpp"

namespace rtype::client {

/**
 * @brief What the front of the casino reads once in its picture to light it:
 * the bulbs of the oval of the pylon, of its columns and of the marquee, its
 * diamonds, left then right, the rows of its golden discs, lit and shaded,
 * its neon script, and the twinkling stars of the sky. Empty, the front
 * shows its picture still.
 */
struct EntranceLights {
  BulbChase oval{CASINO_BRICK_RED};
  BulbChase columns{CASINO_BRICK_RED};
  BulbChase marquee{ENTRANCE_UNLIT_MARQUEE};
  std::array<std::vector<ColoredPixel>, 2> diamonds;
  std::vector<std::vector<ColoredPixel>> litDiscRows;
  std::vector<std::vector<ColoredPixel>> shadedDiscRows;
  std::vector<ColoredPixel> darkScript;
  std::vector<TwinkleGroup> stars;
};

}  // namespace rtype::client
