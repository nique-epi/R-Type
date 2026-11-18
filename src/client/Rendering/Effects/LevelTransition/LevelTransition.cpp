#include "LevelTransition.hpp"
#include <algorithm>
#include "Easing.hpp"
#include "LevelConstants.hpp"
#include "LevelTransitionConstants.hpp"
#include "MissionStartConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How far through the fly out of the ships a transition is with
 * @p left: 0 before it starts, 1 once they are away. */
float flownOutShare(engine::Duration left) {
  const engine::Duration beforeTheAway =
      left - game::LEVEL_TRANSITION_FLY_IN - game::LEVEL_TRANSITION_AWAY;
  return std::clamp(
      1.0F - shareOf(beforeTheAway, game::LEVEL_TRANSITION_FLY_OUT), 0.0F,
      1.0F);
}

/** @returns How far through the fly in of the ships a transition is with
 * @p left: 0 as they start flying in, 1 at its end. */
float flownInShare(engine::Duration left) {
  return std::clamp(1.0F - shareOf(left, game::LEVEL_TRANSITION_FLY_IN), 0.0F,
                    1.0F);
}

}  // namespace

float shipShiftAt(engine::Duration left) {
  if (left <= engine::Duration::zero()) {
    return 0.0F;
  }
  if (left <= game::LEVEL_TRANSITION_FLY_IN) {
    return -LEVEL_TRANSITION_IN_DISTANCE *
           (1.0F - easeOutCubic(flownInShare(left)));
  }
  return LEVEL_TRANSITION_OUT_DISTANCE * easeInCubic(flownOutShare(left));
}

float transitionWarpAt(engine::Duration left) {
  const float extraSpeed = WARP_TOP_SPEED_FACTOR - 1.0F;
  if (left <= engine::Duration::zero()) {
    return 1.0F;
  }
  if (left <= game::LEVEL_TRANSITION_FLY_IN) {
    return 1.0F + (extraSpeed * (1.0F - easeOutCubic(flownInShare(left))));
  }
  return 1.0F + (extraSpeed * flownOutShare(left));
}

}  // namespace rtype::client
