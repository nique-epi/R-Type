#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The filter the tests ask for: two pilots, normal. */
constexpr std::uint8_t GLOBAL_ASKED_PILOTS = 2;
constexpr std::string_view GLOBAL_ASKED_DIFFICULTY = "normal";

/** @brief The rows the panel of the leaderboard holds, as many as it asks the
 * master for. */
constexpr std::int64_t GLOBAL_ROWS_ASKED = 10;

/** @brief The player the tests name, and a nickname the protocol refuses. */
constexpr std::string_view GLOBAL_PLAYER = "NOVA";
constexpr std::string_view EMPTY_NICKNAME{};

/** @brief The leaderboard the master answers: KIRA first, NOVA second and the
 * player asked for, both in a game that ended at GLOBAL_PLAYED_AT, on Paris 1.
 */
constexpr std::string_view GLOBAL_TOP_PILOT = "KIRA";
constexpr std::uint32_t GLOBAL_TOP_SCORE = 1000;
constexpr std::uint32_t GLOBAL_PLAYER_SCORE = 400;
constexpr std::int64_t GLOBAL_FIRST_RANK = 1;
constexpr std::int64_t GLOBAL_SECOND_RANK = 2;
constexpr std::chrono::sys_seconds GLOBAL_PLAYED_AT{
    std::chrono::seconds{1'800'000'000}};
constexpr std::int64_t GLOBAL_SERVER_ID = 7;
constexpr std::string_view GLOBAL_SERVER_NAME = "Paris 1";

/** @brief The same date as the leaderboard screen reads it, in seconds since
 * 1970 UTC. */
constexpr std::int64_t GLOBAL_PLAYED_AT_SECONDS = 1'800'000'000;

}  // namespace rtype::client::testing
