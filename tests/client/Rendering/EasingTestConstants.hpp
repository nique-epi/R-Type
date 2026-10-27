#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief Progresses the easings are read at, and what the mockup's formulas
 * give there. */
constexpr float A_QUARTER = 0.25F;
constexpr float HALF_WAY = 0.5F;
constexpr float TWO_THIRDS = 2.0F / 3.0F;
constexpr float THREE_QUARTERS = 0.75F;
constexpr float CUBE_OF_A_HALF = 0.125F;
constexpr float SWAP_AT_A_QUARTER = 0.0625F;
constexpr float SWAP_AT_THREE_QUARTERS = 0.9375F;
constexpr float BACK_OVERSHOOT = 1.0F + (1.0F / 27.0F);

/** @brief A span of the tests of progressThrough(): one second from 1 s. */
constexpr float SPAN_START = 1.0F;
constexpr float SPAN_LENGTH = 1.0F;

/** @brief The same span on the clock, for progressBetween(), and moments
 * before it, in its middle and after it. */
constexpr rtype::engine::Duration CLOCK_SPAN_START = std::chrono::seconds(1);
constexpr rtype::engine::Duration CLOCK_SPAN_LENGTH = std::chrono::seconds(1);
constexpr rtype::engine::Duration BEFORE_THE_SPAN =
    std::chrono::milliseconds(500);
constexpr rtype::engine::Duration MIDDLE_OF_THE_SPAN =
    std::chrono::milliseconds(1500);
constexpr rtype::engine::Duration AFTER_THE_SPAN = std::chrono::seconds(3);

}  // namespace rtype::client::testing
