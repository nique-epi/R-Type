#pragma once

#include <cstddef>

namespace rtype::server::testing {

/** @brief The health the sheet of the Bydo Tyrant gives, and what the boss
 * has for one to four players: each player past the first adds 25 %. */
constexpr int SHEET_BOSS_HEALTH = 240;
constexpr int BOSS_HEALTH_FOR_ONE_PLAYER = 240;
constexpr int BOSS_HEALTH_FOR_TWO_PLAYERS = 300;
constexpr int BOSS_HEALTH_FOR_THREE_PLAYERS = 360;
constexpr int BOSS_HEALTH_FOR_FOUR_PLAYERS = 420;

/** @brief The health percent of a difficulty that names none, and the one of
 * "hard" in the shipped waves file: 420 × 130 % is 546 for four players, 240
 * × 130 % is 312 for one. */
constexpr int PLAIN_HEALTH_PERCENT = 100;
constexpr int HARD_HEALTH_PERCENT = 130;
constexpr int HARD_BOSS_HEALTH_FOR_ONE_PLAYER = 312;
constexpr int HARD_BOSS_HEALTH_FOR_FOUR_PLAYERS = 546;

constexpr std::size_t NO_PLAYER = 0;
constexpr std::size_t ONE_PLAYER = 1;
constexpr std::size_t TWO_PLAYERS = 2;
constexpr std::size_t THREE_PLAYERS = 3;
constexpr std::size_t FOUR_PLAYERS = 4;

}  // namespace rtype::server::testing
