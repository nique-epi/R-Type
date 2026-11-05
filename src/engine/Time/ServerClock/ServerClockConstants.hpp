#pragma once

#include <cstddef>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::engine {

/** @brief How many of the newest samples the clock keeps to choose from: the
 * eight of the clock filter of NTP (RFC 5905). */
constexpr std::size_t CLOCK_FILTER_WINDOW = 8;

/** @brief The part of the gap to the chosen sample that the clock closes at
 * each sample: one eighth, as the round trip is smoothed. */
constexpr Duration::rep CLOCK_SMOOTHING_DIVISOR = 8;

/** @brief A gap of more than this many ticks is not drift but a change of
 * game or a first reading: the clock jumps to the chosen sample. */
constexpr std::uint32_t CLOCK_SNAP_TICKS = 6;

/** @brief The same gap, as a duration. */
constexpr Duration CLOCK_SNAP_DISTANCE =
    SIMULATION_TICK_DURATION * CLOCK_SNAP_TICKS;

/** @brief Into how many equal parts a round trip splits to give the time of
 * one way, when both ways are taken to last the same. */
constexpr Duration::rep WAYS_OF_A_ROUND_TRIP = 2;

}  // namespace rtype::engine
