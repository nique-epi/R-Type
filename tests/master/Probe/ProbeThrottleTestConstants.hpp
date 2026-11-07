#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

/** @brief A destination, and another port at the same address. */
constexpr std::string_view THROTTLED_ADDRESS = "203.0.113.9";
constexpr std::uint16_t THROTTLED_PORT = 4'242;
constexpr std::uint16_t OTHER_THROTTLED_PORT = 4'243;

/** @brief The least gap the issue allows between two heartbeats of a server,
 * and a millisecond less. */
constexpr std::chrono::milliseconds ONE_SECOND_GAP{1'000};
constexpr std::chrono::milliseconds ALMOST_ONE_SECOND{999};
