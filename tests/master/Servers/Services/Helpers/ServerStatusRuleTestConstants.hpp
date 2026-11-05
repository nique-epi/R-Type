#pragma once

#include <chrono>

/** @brief When the master last heard of the server of a test. */
constexpr std::chrono::sys_seconds HEARD_AT{
    std::chrono::seconds{1'800'000'000}};

/**
 * @brief The silence the issue makes a server offline at, 3 heartbeats of
 * 5 s, and one second less.
 */
constexpr std::chrono::seconds SILENCE_OF_THREE_HEARTBEATS{15};
constexpr std::chrono::seconds ONE_SECOND{1};

/** @brief One tick at 60 ticks a second, in milliseconds, and a little less. */
constexpr double MILLISECONDS_PER_SECOND = 1000.0;
constexpr double TICKS_PER_SECOND = 60.0;
constexpr double ONE_TICK_MILLISECONDS =
    MILLISECONDS_PER_SECOND / TICKS_PER_SECOND;
constexpr double A_LITTLE = 0.01;

/** @brief The loss the issue makes a server degraded at. */
constexpr double FIVE_PERCENT = 5.0;

/** @brief Measures well within the bounds, those of "Paris 1". */
constexpr double QUIET_TICK_P95 = 4.25;
constexpr double QUIET_LOSS = 0.5;
