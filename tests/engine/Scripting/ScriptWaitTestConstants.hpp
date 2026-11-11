#pragma once

#include <cstdint>
#include <limits>

/** @brief Half a second, and the 30 ticks of a sixtieth of a second it
 * lasts. */
constexpr double HALF_A_SECOND_IN_MILLISECONDS = 500.0;
constexpr std::uint32_t TICKS_IN_HALF_A_SECOND = 30;

/** @brief A wait of a tick and a half, 25 ms, which rounds to 2 ticks, and
 * one of 24 ms, under it, which rounds to 1. */
constexpr double A_TICK_AND_A_HALF_IN_MILLISECONDS = 25.0;
constexpr std::uint32_t TICKS_ROUNDED_UP_FROM_ONE_AND_A_HALF = 2;
constexpr double UNDER_A_TICK_AND_A_HALF_IN_MILLISECONDS = 24.0;

/** @brief No wait at all, which still lasts the shortest wait, one tick. */
constexpr double NO_WAIT_IN_MILLISECONDS = 0.0;
constexpr std::uint32_t SHORTEST_WAIT_IN_TICKS = 1;

/** @brief Durations no wait can be. */
constexpr double NEGATIVE_MILLISECONDS = -1.0;
constexpr double ENDLESS_MILLISECONDS = std::numeric_limits<double>::infinity();
