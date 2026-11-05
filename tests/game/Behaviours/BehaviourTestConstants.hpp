#pragma once

#include <chrono>
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @brief The row a piloted body of the tests appears on, and where along it,
 * in logical units. */
constexpr float ROW_Y = 300.0F;
constexpr float BODY_START_X = 800.0F;
constexpr Position ROW_START{.x = BODY_START_X, .y = ROW_Y};

/** @brief The top speed of a Bydo, which the requirements set at 240 logical
 * units per second. */
constexpr float BODY_TOP_SPEED = 240.0F;

/** @brief A quarter and a half of the 2 second period of a weave. */
constexpr engine::Duration QUARTER_PERIOD = std::chrono::milliseconds(500);
constexpr engine::Duration HALF_PERIOD = std::chrono::seconds(1);

/** @brief Where a body on the row at 300 is at the bottom of its weave: 48
 * units lower, as y grows downwards. */
constexpr float WAVE_BOTTOM_Y = 348.0F;

/** @brief The vertical share that takes a body at 240 units per second from
 * its row to the bottom of its weave, 48 units lower, in a quarter period of
 * half a second: 48 / (240 * 0.5). */
constexpr float QUARTER_PERIOD_VERTICAL_SHARE = 0.4F;

constexpr float SHARE_TOLERANCE = 1e-4F;

}  // namespace rtype::game::testing
