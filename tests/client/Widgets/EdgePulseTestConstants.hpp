#pragma once

#include <chrono>
#include <cstdint>
#include "TimeConstants.hpp"

/**
 * @brief The pulse the design sets: from the start of the WARNING screen at
 * 0.6 s, a cosine of 800 ms from clear to 70 % (alpha 178) and back, so at
 * its peak at 1 s and clear again at 1.4 s; the roar flashes the edges at
 * 40 % (alpha 102), half of it (51) 200 ms later, clear 400 ms later; a time
 * long after the WARNING screen.
 */
constexpr rtype::engine::Duration PULSE_START = std::chrono::milliseconds(600);
constexpr rtype::engine::Duration PULSE_PEAK_TIME =
    std::chrono::milliseconds(1000);
constexpr rtype::engine::Duration PULSE_WHOLE_PERIOD_TIME =
    std::chrono::milliseconds(1400);
constexpr std::uint8_t PULSE_PEAK_ALPHA = 178;
constexpr rtype::engine::Duration LONG_AFTER_THE_WARNING =
    std::chrono::seconds(7);
constexpr rtype::engine::Duration AT_THE_ROAR = rtype::engine::Duration::zero();
constexpr rtype::engine::Duration HALFWAY_THROUGH_THE_ROAR_FLASH =
    std::chrono::milliseconds(200);
constexpr rtype::engine::Duration ROAR_FLASH_OVER =
    std::chrono::milliseconds(400);
constexpr std::uint8_t ROAR_FLASH_ALPHA = 102;
constexpr std::uint8_t HALF_THE_ROAR_FLASH_ALPHA = 51;

/** @brief The strip 2 pixels in from the left edge, and the last row the
 * pulse may tint, above the 24 rows of the status band. */
constexpr int SECOND_STRIP_COLUMN = 2;
constexpr int PLAYFIELD_ROWS_OVER_THE_BAND = 246;
