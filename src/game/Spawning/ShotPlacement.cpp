#include "ShotPlacement.hpp"
#include <cstddef>
#include <optional>
#include "BodyPlacement.hpp"
#include "CollisionConstants.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityType.hpp"
#include "ShotPattern.hpp"
#include "ShotStart.hpp"
#include "SpawningConstants.hpp"
#include "VolleyPlacement.hpp"
#include "VolleySheet.hpp"

namespace rtype::game {

namespace {

/** @returns The volley that lays the projectiles of @p pattern fired by a
 * body of side @p side: its rows centred on the start, its fan turned the
 * way that side fires. */
VolleySheet volleyOf(const ShotPattern& pattern, EntityType side) {
  const float rows = static_cast<float>(pattern.count - 1) * pattern.stepY;
  return VolleySheet{.on = {},
                     .count = pattern.count,
                     .offsetX = 0.0F,
                     .offsetY = -rows * engine::HALF,
                     .stepX = 0.0F,
                     .stepY = pattern.stepY,
                     .spreadDegrees = pattern.spreadDegrees,
                     .distance = pattern.distance,
                     .headingDegrees = side == EntityType::Player
                                           ? RIGHTWARD_SHOT_DEGREES
                                           : LEFTWARD_SHOT_DEGREES,
                     .jitterX = 0.0F,
                     .jitterY = 0.0F,
                     .gapFrom = 0,
                     .gapCount = 0,
                     .chancePercent = VOLLEY_SURE_CHANCE,
                     .body = {},
                     .bodyNumber = 0,
                     .bodyPlace = {}};
}

}  // namespace

BodyPlacement shotPlacement(const ShotPattern& pattern, ShotStart start,
                            std::size_t index) {
  const std::optional<BodyPlacement> placement =
      volleyPlacement(volleyOf(pattern, start.side), start.point, index);
  return placement.value_or(
      BodyPlacement{.position = start.point, .origin = start.point});
}

}  // namespace rtype::game
