#include "PixelEffectsFixtures.hpp"
#include "PixelEffectsTestConstants.hpp"
#include "PlacedBurst.hpp"

using rtype::client::PlacedBurst;

PlacedBurst eightDropsAimedLeft() {
  return {.shape = EIGHT_DROPS,
          .center = CENTER,
          .aim = LEFT,
          .colors = RAMP,
          .delay = WAIT};
}
