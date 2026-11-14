#include "BarrelRoll.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <optional>
#include "ManeuverKind.hpp"
#include "RolledRows.hpp"
#include "ShipControlConstants.hpp"
#include "ShipMotion.hpp"

namespace rtype::game {

RolledRows rolledRowsOf(int height, float share) {
  const float cosine = std::cos(ROLL_FULL_TURN * share);
  const int count = static_cast<int>(
      std::lround(static_cast<float>(height) * std::abs(cosine)));
  return RolledRows{.height = height,
                    .count = std::max(ROLL_THINNEST_ROWS, count),
                    .belly = cosine < 0.0F};
}

int sourceRowOf(RolledRows rolled, int row) {
  const int nearest = (((2 * row) + 1) * rolled.height) / (2 * rolled.count);
  return rolled.belly ? rolled.height - 1 - nearest : nearest;
}

std::optional<float> rollShareOf(const ShipMotion& motion) {
  if (motion.maneuver != ManeuverKind::Roll) {
    return std::nullopt;
  }
  const std::chrono::duration<float> time = motion.maneuverTime;
  const std::chrono::duration<float> duration = ROLL_DURATION;
  return std::min(1.0F, time / duration);
}

}  // namespace rtype::game
