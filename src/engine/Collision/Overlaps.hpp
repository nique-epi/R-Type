#pragma once

#include "Bounds.hpp"
#include "CollisionConstants.hpp"

namespace rtype::engine {

/**
 * @brief Tells whether two rectangles share some area.
 *
 * Two rectangles overlap when their interiors intersect: rectangles that only
 * touch along an edge or at a corner do not overlap. A rectangle with no area
 * overlaps another one only if it lies strictly inside it. The result is the
 * same whichever rectangle comes first.
 */
constexpr bool overlaps(const Bounds& first, const Bounds& second) {
  const float firstLeft = first.centerX - (first.width * HALF);
  const float firstRight = first.centerX + (first.width * HALF);
  const float firstTop = first.centerY - (first.height * HALF);
  const float firstBottom = first.centerY + (first.height * HALF);
  const float secondLeft = second.centerX - (second.width * HALF);
  const float secondRight = second.centerX + (second.width * HALF);
  const float secondTop = second.centerY - (second.height * HALF);
  const float secondBottom = second.centerY + (second.height * HALF);
  return firstLeft < secondRight && secondLeft < firstRight &&
         firstTop < secondBottom && secondTop < firstBottom;
}

}  // namespace rtype::engine
