#include "MissionStart.hpp"
#include "Easing.hpp"
#include "MissionStartConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

float warpSpeedFactorAt(engine::Duration sinceStart) {
  if (sinceStart < engine::Duration::zero() || sinceStart >= WARP_DURATION) {
    return 1.0F;
  }
  const float extraSpeed = WARP_TOP_SPEED_FACTOR - 1.0F;
  if (sinceStart < WARP_RISE) {
    return 1.0F + (extraSpeed * shareOf(sinceStart, WARP_RISE));
  }
  const float slowedDown =
      easeOutCubic(shareOf(sinceStart - WARP_RISE, WARP_DURATION - WARP_RISE));
  return 1.0F + (extraSpeed * (1.0F - slowedDown));
}

}  // namespace rtype::client
