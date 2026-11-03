#pragma once

#include <chrono>
#include <cstddef>

namespace rtype::network::testing {

/**
 * @brief Two round trips in a row, and the smoothed round trip the second one
 * leaves: 40 ms moved by an eighth of its gap of 40 ms to 80 ms, as RFC 6298
 * smooths.
 */
constexpr std::chrono::milliseconds FIRST_ROUND_TRIP{40};
constexpr std::chrono::milliseconds SECOND_ROUND_TRIP{80};
constexpr std::chrono::milliseconds SMOOTHED_AFTER_THE_SECOND{45};

/**
 * @brief Three round trips in a row whose gaps are 20 ms and 10 ms, so their
 * jitter is 15 ms.
 */
constexpr std::chrono::milliseconds LOW_ROUND_TRIP{10};
constexpr std::chrono::milliseconds HIGH_ROUND_TRIP{30};
constexpr std::chrono::milliseconds MIDDLE_ROUND_TRIP{20};
constexpr std::chrono::milliseconds JITTER_OF_THE_THREE{15};

/**
 * @brief A spike of 100 ms among calm round trips of 40 ms: a window holding
 * the spike and 15 calm ones has one gap of 60 ms in 15, a jitter of 4 ms.
 */
constexpr std::chrono::milliseconds SPIKE_ROUND_TRIP{100};
constexpr std::chrono::milliseconds CALM_ROUND_TRIP{40};
constexpr std::chrono::milliseconds JITTER_OF_ONE_SPIKE_IN_THE_WINDOW{4};

/**
 * @brief A window of 14 calm round trips of 40 ms, then a spike of 100 ms,
 * then a round trip of 10 ms: gaps of 60 ms and 90 ms in 15, a jitter of
 * 10 ms.
 */
constexpr std::chrono::milliseconds JITTER_OF_A_SPIKE_THEN_A_DROP_IN_THE_WINDOW{
    10};

/**
 * @brief The round trips the jitter is computed on, as RTY-94 asks: the 16
 * newest.
 */
constexpr std::size_t ROUND_TRIPS_IN_THE_WINDOW = 16;

}  // namespace rtype::network::testing
