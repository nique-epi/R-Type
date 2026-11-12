#pragma once

#include <chrono>
#include "HeartbeatTimes.hpp"

namespace rtype::server::testing {

/** @brief A first ceiling of 100 ms, and a maximum the doubling reaches on
 * its fifth wait: 100, 200, 400, 800, then 1000 instead of 1600. */
constexpr HeartbeatTimes BACKOFF_TIMES{
    .interval = std::chrono::milliseconds{5'000},
    .noticeGap = std::chrono::milliseconds{1'000},
    .retry = std::chrono::milliseconds{100},
    .maximumRetry = std::chrono::milliseconds{1'000}};

/** @brief The ceilings of the first waits, as the doubling makes them. */
constexpr std::chrono::milliseconds FIRST_CEILING{100};
constexpr std::chrono::milliseconds SECOND_CEILING{200};
constexpr std::chrono::milliseconds THIRD_CEILING{400};
constexpr std::chrono::milliseconds FOURTH_CEILING{800};
constexpr std::chrono::milliseconds BACKOFF_MAXIMUM{1'000};

/** @brief Draws of the jitter: the whole ceiling, none of it, and a quarter
 * of it, with the wait a quarter of the first ceiling gives. */
constexpr double WHOLE_DRAW = 1.0;
constexpr double NO_DRAW = 0.0;
constexpr double QUARTER_DRAW = 0.25;
constexpr std::chrono::milliseconds QUARTER_OF_FIRST_CEILING{25};
constexpr std::chrono::milliseconds NO_WAIT{0};

}  // namespace rtype::server::testing
