#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief Measures a game reports, in microseconds: 0.8 ms and 24 ms. */
constexpr std::uint32_t TICK_P95_OF_800_MICROSECONDS = 800;
constexpr std::uint32_t ROUND_TRIP_OF_24000_MICROSECONDS = 24000;

/** @brief The line of the game in place 1, game 1 on port 50001, with two
 * pilots, with and without its measures. */
constexpr std::string_view FIRST_GAME_MEASURED_LINE =
    "place 1: game 1, in progress, 2 pilots, port 50001, tick p95 0.8 ms, "
    "ping 24.0 ms";
constexpr std::string_view FIRST_GAME_UNMEASURED_LINE =
    "place 1: game 1, in progress, 2 pilots, port 50001";

/** @brief The pilots of the game in place 1, then of an empty one. */
constexpr std::string_view FIRST_GAME_PILOTS_LINE = "place 1: PILOT1, PILOT2";
constexpr std::string_view SECOND_GAME_NO_PILOT_LINE = "place 2: no pilot";

/** @brief The measures of the games: a median tick of 0.5 ms and 2 % lost. */
constexpr double MEDIAN_TICK_OF_HALF_A_MILLISECOND = 0.5;
constexpr double LOSS_OF_TWO_PERCENT = 2.0;
constexpr std::string_view TWO_MEASURES_LINE =
    "games: measured, tick p50 0.5 ms, loss 2.0 %";

/** @brief Seven rejected datagrams, and the line that counts them. */
constexpr std::size_t SEVEN_REJECTED = 7;
constexpr std::string_view SEVEN_REJECTED_LINE =
    "rejected since the start: 7 datagrams";
