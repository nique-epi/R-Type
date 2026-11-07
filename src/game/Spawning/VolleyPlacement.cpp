#include "VolleyPlacement.hpp"
#include <cmath>
#include <cstddef>
#include <optional>
#include "BodyPlacement.hpp"
#include "CollisionConstants.hpp"
#include "DirectionConstants.hpp"
#include "Position.hpp"
#include "VolleySheet.hpp"

namespace rtype::game {

namespace {

/** @returns The heading of body @p index of @p volley, in degrees from the
 * left: spread evenly over the volley, 0 for a volley of one body. */
float headingDegreesOf(const VolleySheet& volley, std::size_t index) {
  if (volley.count <= 1) {
    return 0.0F;
  }
  return (-volley.spreadDegrees * engine::HALF) +
         (volley.spreadDegrees * static_cast<float>(index) /
          static_cast<float>(volley.count - 1));
}

/** @returns Whether body @p index of @p volley is in its gap. */
bool isInTheGap(const VolleySheet& volley, std::size_t index) {
  return index >= volley.gapFrom &&
         index < static_cast<std::size_t>(volley.gapFrom) + volley.gapCount;
}

}  // namespace

std::optional<BodyPlacement> volleyPlacement(const VolleySheet& volley,
                                             Position center,
                                             std::size_t index) {
  if (isInTheGap(volley, index)) {
    return std::nullopt;
  }
  const auto steps = static_cast<float>(index);
  const Position origin{
      .x = center.x + volley.offsetX + (volley.stepX * steps),
      .y = center.y + volley.offsetY + (volley.stepY * steps)};
  const float radians = headingDegreesOf(volley, index) * RADIANS_PER_DEGREE;
  return BodyPlacement{
      .position =
          Position{.x = origin.x - (volley.distance * std::cos(radians)),
                   .y = origin.y + (volley.distance * std::sin(radians))},
      .origin = origin};
}

}  // namespace rtype::game
