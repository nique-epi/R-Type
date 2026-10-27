#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/**
 * @brief The 500 ms the protocol page sets between two GameEnded while a game
 * is finished, at 60 ticks per second.
 */
constexpr std::size_t PROTOCOL_GAME_ENDED_TICKS = 30;

/**
 * @brief The numbers the first and the second player of a test join under: the
 * lowest a roster gives.
 */
constexpr std::uint8_t FIRST_JOINED_NUMBER = 1;
constexpr std::uint8_t SECOND_JOINED_NUMBER = 2;

/**
 * @brief Scores of the first and the second player when their game ends,
 * different so that a swap shows.
 */
constexpr std::uint32_t FIRST_PLAYER_FINAL_SCORE = 1200;
constexpr std::uint32_t SECOND_PLAYER_FINAL_SCORE = 300;

/** @brief Points a player is credited after its game ended. */
constexpr std::uint32_t POINTS_AFTER_THE_END = 500;

}  // namespace rtype::server::testing
