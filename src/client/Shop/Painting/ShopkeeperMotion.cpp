#include "ShopkeeperMotion.hpp"
#include <cstddef>
#include "ShopkeeperEye.hpp"
#include "ShopkeeperEyeCue.hpp"
#include "ShopkeeperMotionConstants.hpp"
#include "ShopkeeperPose.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The eye at @p step of its cycle: the one of the last cue at or
 * before that step. */
ShopkeeperEye eyeAt(std::size_t step) {
  ShopkeeperEye eye = ShopkeeperEye::Ahead;
  for (const ShopkeeperEyeCue& cue : SHOPKEEPER_EYE_CUES) {
    if (cue.step > step) {
      break;
    }
    eye = cue.eye;
  }
  return eye;
}

}  // namespace

ShopkeeperPose shopkeeperPoseAt(engine::Duration elapsed) {
  const auto step = static_cast<std::size_t>(elapsed / SHOPKEEPER_STEP);
  return ShopkeeperPose{
      .drop = SHOPKEEPER_DROPS.at(step % SHOPKEEPER_DROPS.size()),
      .eye = eyeAt(step % SHOPKEEPER_EYE_CYCLE_STEPS)};
}

}  // namespace rtype::client
