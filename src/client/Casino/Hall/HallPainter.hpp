#pragma once

#include <vector>
#include "ColoredPixel.hpp"
#include "HallMoment.hpp"
#include "HallPicture.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief Paints the main hall of the casino from its picture: the spotlight
 * and the beams over the ring, the chasing bulbs, the dimming crystals and
 * the slot machines, the boxers sparring behind the front ropes, the crowd
 * hopping and its camera flashes, the chevrons toward the roulette, then its
 * title. With MotionPreference::Reduced, the picture stays still and the
 * boxers on guard.
 *
 * It keeps the pixels of the beams between two paints, so the sweep of the
 * beams builds no list per frame.
 */
class HallPainter {
 public:
  explicit HallPainter(HallPicture picture);

  void paint(IInterfaceCanvas& canvas, HallMoment moment);

 private:
  /** @brief Paints the two beams that sweep the ceiling. */
  void paintBeams(IInterfaceCanvas& canvas, engine::Duration since);

  HallPicture picture_;
  std::vector<ColoredPixel> beamPixels_;
};

}  // namespace rtype::client
