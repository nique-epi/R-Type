#include "EntranceLightPainter.hpp"
#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>
#include "CasinoLightConstants.hpp"
#include "CasinoLighting.hpp"
#include "ColoredPixel.hpp"
#include "EntranceLightConstants.hpp"
#include "EntranceLights.hpp"
#include "IInterfaceCanvas.hpp"
#include "IntroTimeline.hpp"
#include "LightBeam.hpp"
#include "TimeConstants.hpp"
#include "TwinkleGroup.hpp"

namespace rtype::client {

namespace {

/** @returns How long a whole round of the waits of the neon script lasts. */
engine::Duration neonRound() {
  engine::Duration round{};
  for (const engine::Duration wait : ENTRANCE_NEON_WAITS) {
    round += wait;
  }
  return round;
}

/** @returns Whether the neon script is cut out @p since the front opened:
 * during the cuts that open each of its waits. */
bool neonCutAt(engine::Duration since) {
  engine::Duration left = since % neonRound();
  for (std::size_t wait = 0; wait < ENTRANCE_NEON_WAITS.size(); ++wait) {
    if (left < ENTRANCE_NEON_WAITS.at(wait)) {
      const auto cuts = static_cast<engine::Duration::rep>(
          ENTRANCE_FEWEST_NEON_CUTS +
          static_cast<int>(wait % ENTRANCE_NEON_CUT_CHOICES));
      return left < ENTRANCE_NEON_CUT * cuts * ENTRANCE_NEON_CUT_PHASES &&
             (left / ENTRANCE_NEON_CUT) % ENTRANCE_NEON_CUT_PHASES == 0;
    }
    left -= ENTRANCE_NEON_WAITS.at(wait);
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
  paintSearchlights(canvas, since);
  paintPylon(canvas, lights_, since);
  canvas.plot(lights_.marquee.unlitAt(-chaseStepAt(since)));
}

void EntranceLightPainter::paintSearchlights(IInterfaceCanvas& canvas,
                                             engine::Duration since) {
  const float swing =
      std::sin(secondsOf(since) * FULL_TURN_RADIANS / ENTRANCE_BEAM_PERIOD) *
      ENTRANCE_BEAM_SWING;
  beamPixels_.clear();
  addBeamPixels(beamPixels_, LightBeam{.origin = ENTRANCE_LEFT_BEAM_ORIGIN,
                                       .angle = ENTRANCE_LEFT_BEAM_REST + swing,
                                       .length = ENTRANCE_BEAM_LENGTH,
                                       .halfSpread = ENTRANCE_BEAM_HALF_SPREAD,
                                       .color = ENTRANCE_BEAM});
  addBeamPixels(beamPixels_,
                LightBeam{.origin = ENTRANCE_RIGHT_BEAM_ORIGIN,
                          .angle = ENTRANCE_RIGHT_BEAM_REST - swing,
                          .length = ENTRANCE_BEAM_LENGTH,
                          .halfSpread = ENTRANCE_BEAM_HALF_SPREAD,
                          .color = ENTRANCE_BEAM});
  canvas.plot(beamPixels_);
}

}  // namespace rtype::client
