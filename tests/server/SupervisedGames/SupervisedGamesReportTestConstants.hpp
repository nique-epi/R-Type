#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief What the master is told of the game in place 1. */
constexpr std::string_view NAME_OF_THE_GAME_IN_PLACE_1 = "GAME 1";
constexpr std::string_view NAME_OF_THE_GAME_IN_PLACE_2 = "GAME 2";
constexpr std::string_view DIFFICULTY_OF_EVERY_GAME = "normal";

/** @brief The players of a full game, of a game that waits with one, of a game
 * in progress with two, and of a busy game with three. */
constexpr std::size_t FOUR_PLAYERS = 4;
constexpr std::size_t ONE_PLAYER = 1;
constexpr std::size_t TWO_PLAYERS = 2;
constexpr std::size_t THREE_PLAYERS = 3;

/** @brief Two games: the first ticks at 2 ms at the median and 5 ms at the
 * 95th percentile, the second at 3 ms and 4 ms; the slowest are 3 ms and
 * 5 ms. */
constexpr std::uint32_t FIRST_GAME_MEDIAN_MICROSECONDS = 2000;
constexpr std::uint32_t FIRST_GAME_P95_MICROSECONDS = 5000;
constexpr std::uint32_t SECOND_GAME_MEDIAN_MICROSECONDS = 3000;
constexpr std::uint32_t SECOND_GAME_P95_MICROSECONDS = 4000;
constexpr double SLOWEST_MEDIAN_MILLISECONDS = 3.0;
constexpr double SLOWEST_P95_MILLISECONDS = 5.0;

/** @brief A game of one player 10 ms away and a game of three players 30 ms
 * away on average: (10 + 3 * 30) / 4 = 25 ms for the four players. */
constexpr std::uint32_t NEAR_GAME_ROUND_TRIP_MICROSECONDS = 10000;
constexpr std::uint32_t FAR_GAME_ROUND_TRIP_MICROSECONDS = 30000;
constexpr double WEIGHTED_ROUND_TRIP_MILLISECONDS = 25.0;

/** @brief An idle supervisor: eight games it may run, 32 seats; one waiting
 * game with one player leaves 31; a game of two in progress leaves the 28
 * seats of the seven other places. */
constexpr std::uint16_t GAMES_A_SUPERVISOR_RUNS = 8;
constexpr std::uint16_t SEATS_OF_AN_IDLE_SUPERVISOR = 32;
constexpr std::uint16_t SEATS_BESIDE_ONE_WAITING_PLAYER = 31;
constexpr std::uint16_t SEATS_OF_SEVEN_FREE_PLACES = 28;
