#pragma once

#include <chrono>

namespace rtype::server::testing {

/** @brief The duration of the slowest tick of the full buffer the tests fill,
 * one millisecond more for each tick: 1, 2, ... 600 ms. */
constexpr int SLOWEST_TICK_MILLISECONDS = 600;

/** @brief What the nearest rank gives of those 600 ticks: rank 300 for the
 * median, rank 570 for the 95th percentile. Their average is (1 + 600) / 2. */
constexpr std::chrono::milliseconds FULL_BUFFER_MEDIAN{300};
constexpr std::chrono::milliseconds FULL_BUFFER_NINETY_FIFTH_PERCENTILE{570};
constexpr std::chrono::milliseconds FULL_BUFFER_WORST{600};
constexpr std::chrono::microseconds FULL_BUFFER_AVERAGE{300500};

/** @brief A tick slower than any of those 600. */
constexpr std::chrono::milliseconds SLOWER_THAN_THE_FULL_BUFFER{601};

/** @brief The two ticks of a short series, and their average. */
constexpr std::chrono::milliseconds SHORT_TICK{1};
constexpr std::chrono::milliseconds LONG_TICK{2};
constexpr std::chrono::microseconds SHORT_AND_LONG_AVERAGE{1500};

}  // namespace rtype::server::testing
