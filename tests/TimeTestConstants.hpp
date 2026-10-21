#pragma once

#include <chrono>

/**
 * @brief The shortest step of time a test takes: a delay less one nanosecond
 * is still before that delay.
 */
constexpr std::chrono::nanoseconds ONE_NANOSECOND{1};

namespace rtype::engine::testing {

/** @brief One second of simulation time, as an engine::Duration, for the tests
 * that measure what moves or changes in that time. */
constexpr std::chrono::nanoseconds ONE_SECOND = std::chrono::seconds(1);

}  // namespace rtype::engine::testing
