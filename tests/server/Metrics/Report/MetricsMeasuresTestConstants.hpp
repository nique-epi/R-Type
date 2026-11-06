#pragma once

#include <chrono>
#include <cstddef>

namespace rtype::server::testing {

/** @brief Ticks of 2 ms at the median and 5 ms at the 95th percentile. */
constexpr std::chrono::milliseconds TWO_MILLISECOND_MEDIAN_TICK{2};
constexpr std::chrono::milliseconds FIVE_MILLISECOND_SLOW_TICK{5};
constexpr double TWO_MILLISECONDS = 2.0;
constexpr double FIVE_MILLISECONDS = 5.0;
constexpr std::size_t TICKS_OF_TEN_SECONDS = 600;

/** @brief Two players 20 ms and 40 ms away: 30 ms on average. */
constexpr std::chrono::milliseconds TWENTY_MILLISECOND_ROUND_TRIP{20};
constexpr std::chrono::milliseconds FORTY_MILLISECOND_ROUND_TRIP{40};
constexpr double THIRTY_MILLISECONDS = 30.0;

}  // namespace rtype::server::testing
