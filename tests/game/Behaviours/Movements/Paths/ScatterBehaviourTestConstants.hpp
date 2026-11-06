#pragma once

#include <numbers>
#include "Position.hpp"

namespace rtype::game::testing {

/** @brief A shard that appeared 8 units up and to the right of the center it
 * flies away from, and the share of its speed it takes on each axis then:
 * the cosine of 45 degrees. */
constexpr Position BURST_CENTER{.x = 960.0F, .y = 540.0F};
constexpr Position UP_RIGHT_OF_THE_CENTER{.x = 965.65685F, .y = 534.34315F};
constexpr float DIAGONAL_SHARE = std::numbers::sqrt2_v<float> / 2.0F;
constexpr float SCATTER_SHARE_TOLERANCE = 1e-4F;

}  // namespace rtype::game::testing
