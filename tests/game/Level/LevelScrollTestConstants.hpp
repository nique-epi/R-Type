#pragma once

#include <chrono>
#include "TimeConstants.hpp"

/** @brief One second of scroll, and how far a level scrolls in it: 120
 * logical units. */
constexpr rtype::engine::Duration ONE_SECOND_OF_SCROLL =
    std::chrono::seconds(1);
constexpr float UNITS_SCROLLED_IN_ONE_SECOND = 120.0F;

/** @brief Two seconds of scroll, and how far a level scrolls in them. */
constexpr float UNITS_SCROLLED_IN_TWO_SECONDS = 240.0F;

/** @brief The speed of a scroll that moves, in logical units per second, and
 * of one that is paused. */
constexpr float SPEED_OF_A_MOVING_SCROLL = 120.0F;
constexpr float SPEED_OF_A_PAUSED_SCROLL = 0.0F;

/** @brief A time that runs backwards. */
constexpr rtype::engine::Duration ONE_SECOND_BACKWARDS =
    std::chrono::seconds(-1);

/** @brief Where a level starts. */
constexpr float START_OF_THE_LEVEL = 0.0F;
