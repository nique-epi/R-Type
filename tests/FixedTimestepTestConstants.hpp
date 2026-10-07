#pragma once

#include <cstddef>
#include <cstdint>
#include "TimeConstants.hpp"

/**
 * @brief Nanoseconds in one second, to split a second into whole frames.
 */
constexpr std::int64_t NANOSECONDS_PER_SECOND = 1'000'000'000;

/**
 * @brief Speed of the ship the frame rate tests move.
 */
constexpr double SPEED_UNITS_PER_SECOND = 120.0;

/**
 * @brief Gap allowed on the position after one simulated second: the tick
 * length is a whole number of nanoseconds, so 60 ticks fall 0.4 parts per
 * million short of one second.
 */
constexpr double POSITION_TOLERANCE_UNITS = 1e-4;

/**
 * @brief Alignment of the result a frame rate test returns.
 */
constexpr std::size_t RESULT_ALIGNMENT = 16;

/**
 * @brief A tick short enough to split into exact halves and quarters.
 */
constexpr rtype::engine::Duration SHORT_TICK_DURATION(100);

/**
 * @brief Half of SHORT_TICK_DURATION.
 */
constexpr rtype::engine::Duration HALF_SHORT_TICK(SHORT_TICK_DURATION / 2);
