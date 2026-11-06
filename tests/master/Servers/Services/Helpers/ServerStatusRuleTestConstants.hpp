#pragma once

#include <chrono>

/** @brief When the master last heard of the server of a test, and a minute
 * before. */
constexpr std::chrono::sys_seconds HEARD_AT{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::seconds ONE_MINUTE{60};

/** @brief The silence the issue makes a server offline at, 3 heartbeats of
 * 5 s. */
constexpr std::chrono::seconds SILENCE_OF_THREE_HEARTBEATS{15};

/** @brief The time without a valid token of a probe the issue makes a server
 * unreachable at. */
constexpr std::chrono::seconds UNANSWERED_FOR_FIFTEEN_SECONDS{15};

/** @brief One tick at 60 ticks a second, in milliseconds, and a little less. */
constexpr double MILLISECONDS_PER_SECOND = 1000.0;
constexpr double TICKS_PER_SECOND = 60.0;
constexpr double ONE_TICK_MILLISECONDS =
    MILLISECONDS_PER_SECOND / TICKS_PER_SECOND;
constexpr double A_LITTLE = 0.01;

/** @brief The loss the issue makes a server degraded at. */
constexpr double FIVE_PERCENT = 5.0;
