#pragma once

#include <chrono>

/**
 * @brief The shortest step of time a test takes: a delay less one nanosecond
 * is still before that delay.
 */
constexpr std::chrono::nanoseconds ONE_NANOSECOND{1};
