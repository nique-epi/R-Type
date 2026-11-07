#include "BeamOverlap.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include "BeamLine.hpp"
#include "Bounds.hpp"
#include "CollisionConstants.hpp"
#include "Direction.hpp"

namespace rtype::game {

namespace {

/** @returns The dot product of @p first and @p second. */
float dot(const Direction& first, const Direction& second) {
  return (first.x * second.x) + (first.y * second.y);
}

/** @returns The way across @p beam, a quarter turn from its heading. */
Direction acrossOf(const BeamLine& beam) {
  return Direction{.x = -beam.heading.y, .y = beam.heading.x};
}

/** @returns Whether @p axis separates @p beam from @p box: the gap between
 * their centers along it is at least the sum of their half extents along
 * it. */
bool separates(const BeamLine& beam, const engine::Bounds& box,
               const Direction& axis) {
  const float halfLength = beam.length * engine::HALF;
  const Direction gap{
      .x = beam.startX + (beam.heading.x * halfLength) - box.centerX,
      .y = beam.startY + (beam.heading.y * halfLength) - box.centerY};
  const float boxReach = (box.width * engine::HALF * std::abs(axis.x)) +
                         (box.height * engine::HALF * std::abs(axis.y));
  const float beamReach =
      (halfLength * std::abs(dot(beam.heading, axis))) +
      (beam.thickness * engine::HALF * std::abs(dot(acrossOf(beam), axis)));
  return std::abs(dot(gap, axis)) >= boxReach + beamReach;
}

}  // namespace

bool beamOverlaps(const BeamLine& beam, const engine::Bounds& box) {
  const std::array axes{Direction{.x = 1.0F, .y = 0.0F},
                        Direction{.x = 0.0F, .y = 1.0F}, beam.heading,
                        acrossOf(beam)};
  return std::ranges::none_of(
      axes, [&](const Direction& axis) { return separates(beam, box, axis); });
}

}  // namespace rtype::game
