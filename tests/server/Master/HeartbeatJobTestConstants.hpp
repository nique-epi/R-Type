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
constexpr std::uint64_t LINKED_PROBE_SECRET = 0x0123'4567'89AB'CDEF;

/** @brief A token of a probe, how the master keeps it in the text of the
 * last heartbeat, and the member whose absence says a heartbeat has none. */
constexpr std::uint64_t LINKED_PROBE_TOKEN = 4'242'424'242;
constexpr std::string_view LINKED_PROBE_TOKEN_TEXT =
    "\"probeToken\":4242424242";
constexpr std::string_view PROBE_TOKEN_MEMBER_TEXT = "probeToken";
/** @brief What every heartbeat the master keeps holds. */
constexpr std::string_view GAMES_MEMBER_TEXT = "\"games\"";
constexpr std::string_view LINKED_DIFFICULTY = "normal";

/** @brief How a heartbeat writes the difficulties of the linked server. */
constexpr std::string_view LINKED_DIFFICULTIES_TEXT =
    R"("difficulties":["normal"])";
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

/** @brief The file of results of the games, in the folder of the test. */
constexpr std::string_view LINKED_RESULTS_FILE = "results.db";

/** @brief When the games the tests save ended, one after the other. A game
 * of no player at its start is one the master refuses for good: it keeps 1 to
 * 4. */
constexpr std::chrono::sys_seconds GAME_END{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::sys_seconds LATER_GAME_END =
    GAME_END + std::chrono::minutes{1};
constexpr std::uint8_t NO_PILOT_AT_THE_START = 0;
constexpr int ONE_ROW = 1;
constexpr int NO_ROW = 0;

/** @brief How long a test leaves the job facing a stopped master. */
constexpr std::chrono::milliseconds MASTER_DOWN_TIME{300};

}  // namespace rtype::server::testing
