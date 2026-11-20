#include "EntranceLightPainter.hpp"
#include <cstddef>
#include <utility>
#include <vector>
#include "CasinoLightConstants.hpp"
#include "CasinoLighting.hpp"
#include "ColoredPixel.hpp"
#include "EntranceLightConstants.hpp"
#include "EntranceLights.hpp"
#include "IInterfaceCanvas.hpp"
#include "NeonCut.hpp"
#include "TimeConstants.hpp"
#include "TwinkleGroup.hpp"

namespace rtype::client {

namespace {

/** @returns Whether the neon script is cut out @p since the front opened:
 * during the cuts that open each of its waits, one off step in two. */
bool neonCutAt(engine::Duration since) {
  engine::Duration left = since % ENTRANCE_NEON_ROUND;
  for (const NeonCut& neon : ENTRANCE_NEON_CUTS) {
    if (left < neon.wait) {
      const auto steps = left / ENTRANCE_NEON_CUT;
      return steps < static_cast<engine::Duration::rep>(neon.cuts) *
                         ENTRANCE_NEON_CUT_PHASES &&
             steps % ENTRANCE_NEON_CUT_PHASES == 0;
    }
    left -= neon.wait;
  }
  return false;
}

/** @brief Paints the rows of the discs: the two rows the wave lights now
 * glow, the others sink. */
void paintDiscs(IInterfaceCanvas& canvas, const EntranceLights& lights,
                engine::Duration since) {
  const auto rows = static_cast<int>(lights.litDiscRows.size());
  const int wave = static_cast<int>((since / ENTRANCE_DISC_STEP) %
                                    (rows + ENTRANCE_DISC_WAVE_PAUSE));
  for (int row = 0; row < rows; ++row) {
    const auto place = static_cast<std::size_t>(row);
    canvas.plot(row == wave || row == wave - 1
                    ? lights.litDiscRows.at(place)
                    : lights.shadedDiscRows.at(place));
  }
}

/** @brief Paints the bulbs of the pylon: the oval chasing around 777, the
 * columns climbing, the diamonds of one side out, the discs, and the script
 * when it cuts out. */
void paintPylon(IInterfaceCanvas& canvas, const EntranceLights& lights,
                engine::Duration since) {
  const int step = chaseStepAt(since);
  canvas.plot(lights.oval.unlitAt(step));
  canvas.plot(lights.columns.unlitAt(-step));
  const auto blinks = static_cast<std::size_t>(since / ENTRANCE_DIAMOND_STEP);
  canvas.plot(lights.diamonds.at((blinks + 1) % lights.diamonds.size()));
  paintDiscs(canvas, lights, since);
  if (neonCutAt(since)) {
    canvas.plot(lights.darkScript);
  }
}

}  // namespace

EntranceLightPainter::EntranceLightPainter(EntranceLights lights)
    : lights_(std::move(lights)) {}

void EntranceLightPainter::paint(IInterfaceCanvas& canvas,
                                 engine::Duration since) {
  for (const TwinkleGroup& stars : lights_.stars) {
    if (isDimmedAt(stars, since)) {
      canvas.plot(stars.pixels);
    }
  }
  swingBeams(beamPixels_, ENTRANCE_SEARCHLIGHTS, since);
  canvas.plot(beamPixels_);
  paintPylon(canvas, lights_, since);
  canvas.plot(lights_.marquee.unlitAt(-chaseStepAt(since)));
}

}  // namespace rtype::client
