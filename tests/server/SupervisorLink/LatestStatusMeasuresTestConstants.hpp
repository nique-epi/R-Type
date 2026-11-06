#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/** @brief Ticks of 2 ms at the median and 5 ms at the 95th percentile, over ten
 * seconds. */
constexpr std::chrono::milliseconds TWO_MILLISECOND_MEDIAN_TICK{2};
constexpr std::chrono::milliseconds FIVE_MILLISECOND_SLOW_TICK{5};
constexpr std::uint32_t TWO_MILLISECONDS_IN_MICROSECONDS = 2000;
constexpr std::uint32_t FIVE_MILLISECONDS_IN_MICROSECONDS = 5000;
constexpr std::size_t TICKS_OF_TEN_SECONDS = 600;

/** @brief Two players 20 ms and 40 ms away: 30 ms on average. */
constexpr std::chrono::milliseconds TWENTY_MILLISECOND_ROUND_TRIP{20};
constexpr std::chrono::milliseconds FORTY_MILLISECOND_ROUND_TRIP{40};
constexpr std::uint32_t THIRTY_MILLISECONDS_IN_MICROSECONDS = 30000;

/** @brief A player 7 ms away, in a report published after the first. */
constexpr std::chrono::milliseconds SEVEN_MILLISECOND_ROUND_TRIP{7};
constexpr std::uint32_t SEVEN_MILLISECONDS_IN_MICROSECONDS = 7000;

}  // namespace rtype::server::testing
