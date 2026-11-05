#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>
#include "HeartbeatTimes.hpp"

namespace rtype::server::testing {

/** @brief The server the job registers, and the difficulty of its game. */
constexpr std::string_view LINKED_NAME = "Paris 1";
constexpr std::uint16_t LINKED_PORT = 4242;
constexpr std::uint16_t LINKED_GAMES = 1;
constexpr std::string_view LINKED_DIFFICULTY = "normal";
constexpr std::uint16_t LINKED_GAME_NUMBER = 1;
constexpr std::string_view LINKED_GAME_NAME = "GAME 1";

/** @brief Two pilots of the game, the second one arriving later. */
constexpr std::string_view FIRST_PILOT_NAME = "ACE";
constexpr std::string_view SECOND_PILOT_NAME = "R9";

/** @brief Delays short enough for a test, and an interval no test waits for,
 * so a heartbeat seen within NOTICE_DEADLINE was sent for a change. */
constexpr HeartbeatTimes FAST_TIMES{.interval = std::chrono::milliseconds{100},
                                    .noticeGap = std::chrono::milliseconds{10},
                                    .retry = std::chrono::milliseconds{50}};
constexpr HeartbeatTimes SLOW_INTERVAL_TIMES{
    .interval = std::chrono::milliseconds{30'000},
    .noticeGap = std::chrono::milliseconds{10},
    .retry = std::chrono::milliseconds{50}};
constexpr std::chrono::milliseconds NOTICE_DEADLINE{5'000};
constexpr std::chrono::milliseconds POLL_STEP{10};

/** @brief How long a test leaves the job facing a stopped master. */
constexpr std::chrono::milliseconds MASTER_DOWN_TIME{300};

}  // namespace rtype::server::testing
