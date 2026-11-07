#include "VolleyPlacementFixtures.hpp"
#include <cstdint>
#include "VolleyPlacementTestConstants.hpp"
#include "VolleySheet.hpp"

namespace rtype::game::testing {

VolleySheet burstOf(std::uint8_t count) {
  return VolleySheet{.count = count,
                     .offsetX = BURST_OFFSET_X,
                     .offsetY = BURST_OFFSET_Y,
                     .spreadDegrees = BURST_SPREAD_DEGREES,
                     .distance = BURST_DISTANCE,
                     .body = {},
                     .bodyPlace = {}};
}

VolleySheet column() {
  return VolleySheet{.count = COLUMN_COUNT,
                     .offsetX = COLUMN_OFFSET_X,
                     .stepY = COLUMN_STEP_Y,
                     .body = {},
                     .bodyPlace = {}};
}

VolleySheet wallWithAGap() {
  return VolleySheet{.count = WALL_COUNT,
                     .offsetX = COLUMN_OFFSET_X,
                     .stepY = COLUMN_STEP_Y,
                     .gapFrom = WALL_GAP_FROM,
                     .gapCount = WALL_GAP_COUNT,
                     .body = {},
                     .bodyPlace = {}};
}

}  // namespace rtype::game::testing
