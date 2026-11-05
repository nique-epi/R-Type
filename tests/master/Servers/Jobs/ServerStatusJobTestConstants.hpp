#pragma once

#include <chrono>
#include <string_view>

/** @brief The time between two refreshes in the tests, and one so long that
 * a stop that waited for it would fail the test. */
constexpr std::chrono::milliseconds SHORT_REFRESH_INTERVAL{10};
constexpr std::chrono::milliseconds ONE_HOUR_INTERVAL{3'600'000};

/** @brief How long a test waits for a status, how often it looks, and how
 * long a stop may take. */
constexpr std::chrono::milliseconds STATUS_DEADLINE{2'000};
constexpr std::chrono::milliseconds STATUS_POLL{5};
constexpr std::chrono::milliseconds STOP_DEADLINE{1'000};

/** @brief The address the server of a test is created from. */
constexpr std::string_view JOB_CREATOR_ADDRESS = "192.0.2.1";
