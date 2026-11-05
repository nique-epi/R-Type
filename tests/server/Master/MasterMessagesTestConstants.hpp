#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The server the messages describe. */
constexpr std::string_view LINKED_SERVER_NAME = "Paris 1";
constexpr std::uint16_t LINKED_SERVER_PORT = 4242;
constexpr std::uint16_t LINKED_SERVER_GAMES = 8;

/** @brief What the server announces of the protocol and of its games. */
constexpr std::uint8_t ANNOUNCED_PROTOCOL_VERSION = 2;
constexpr std::uint8_t ANNOUNCED_PLAYERS_PER_GAME = 4;

/** @brief Ticks of 2 ms at the median and 5 ms at the 95th percentile. */
constexpr std::chrono::milliseconds MEDIAN_TICK{2};
constexpr std::chrono::milliseconds SLOW_TICK{5};
constexpr double MEDIAN_TICK_MILLISECONDS = 2.0;
constexpr double SLOW_TICK_MILLISECONDS = 5.0;
constexpr std::size_t MEASURED_TICKS = 600;

/** @brief Two players 20 ms and 40 ms away: 30 ms on average. */
constexpr std::chrono::milliseconds NEAR_PLAYER_ROUND_TRIP{20};
constexpr std::chrono::milliseconds FAR_PLAYER_ROUND_TRIP{40};
constexpr double AVERAGE_PING_MILLISECONDS = 30.0;

}  // namespace rtype::server::testing
