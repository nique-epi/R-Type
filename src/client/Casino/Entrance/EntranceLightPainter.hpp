#pragma once

#include <vector>
#include "ColoredPixel.hpp"
#include "EntranceLights.hpp"
#include "IInterfaceCanvas.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Brings the front of the casino to life over its picture: the stars
 * twinkle, two searchlights sweep the sky, the bulbs of the oval of the 777
 * pylon chase around it, its columns climb, its diamonds blink in turn, a
 * wave falls down its discs, its neon script cuts out now and then, and the
 * bulbs of the marquee chase from left to right. The numerals 777 and the
 * letters of the marquee stay lit.
 *
 * It keeps the pixels of the searchlights between two paints, so their sweep
 * builds no list per frame.
 */
class EntranceLightPainter {
 public:
  explicit EntranceLightPainter(EntranceLights lights);

  /** @brief Paints the lights as they are @p since the front opened. */
  void paint(IInterfaceCanvas& canvas, engine::Duration since);

 private:
  EntranceLights lights_;
  std::vector<ColoredPixel> beamPixels_;
};

}  // namespace rtype::client
