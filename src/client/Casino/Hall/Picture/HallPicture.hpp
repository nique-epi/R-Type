#pragma once

#include <vector>
#include "BulbChase.hpp"
#include "CasinoHallConstants.hpp"
#include "ColoredPixel.hpp"
#include "Spectator.hpp"
#include "TwinkleGroup.hpp"

namespace rtype::client {

/**
 * @brief What the hall reads once in its picture to bring it to life: the
 * bulbs that chase, the crystals of the chandeliers that dim, the front ropes
 * of the ring painted again over the boxers, and the spectators who hop.
 * Empty, the hall shows its picture still, the boxers in front of the ropes.
 */
struct HallPicture {
  BulbChase bulbs{HALL_UNLIT_BULB};
  std::vector<TwinkleGroup> crystals;
  std::vector<ColoredPixel> frontRopes;
  std::vector<Spectator> spectators;
};

}  // namespace rtype::client
