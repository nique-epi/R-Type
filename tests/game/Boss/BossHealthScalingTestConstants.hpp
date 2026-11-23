#pragma once

#include <cstddef>

namespace rtype::game::testing {

/** @brief The health a sheet gives a boss, and what the boss has for one to
 * four players: each player past the first adds 75 %. */
constexpr int SHEET_BOSS_HEALTH = 240;
constexpr int BOSS_HEALTH_FOR_ONE_PLAYER = 240;
constexpr int BOSS_HEALTH_FOR_TWO_PLAYERS = 420;
constexpr int BOSS_HEALTH_FOR_THREE_PLAYERS = 600;
constexpr int BOSS_HEALTH_FOR_FOUR_PLAYERS = 780;

/** @brief The health percent of a difficulty that names none, and the one of
 * "hard" in the shipped waves file: 780 × 130 % is 1014 for four players,
 * 240 × 130 % is 312 for one. */
constexpr int PLAIN_HEALTH_PERCENT = 100;
constexpr int HARD_HEALTH_PERCENT = 130;
constexpr int HARD_BOSS_HEALTH_FOR_ONE_PLAYER = 312;
constexpr int HARD_BOSS_HEALTH_FOR_FOUR_PLAYERS = 1014;

/** @brief A threat of none, of half the most and of the most, and the 240
 * points of the boss for one player then: 175 % at half, 250 % at the
 * most. */
constexpr int NO_THREAT_PERCENT = 0;
constexpr int HALF_THE_MOST_THREAT_PERCENT = 50;
constexpr int THE_MOST_THREAT_PERCENT = 100;
constexpr int BOSS_HEALTH_AT_HALF_THE_MOST_THREAT = 420;
constexpr int BOSS_HEALTH_AT_THE_MOST_THREAT = 600;

constexpr std::size_t NO_PLAYER = 0;
constexpr std::size_t ONE_PLAYER = 1;
constexpr std::size_t TWO_PLAYERS = 2;
constexpr std::size_t THREE_PLAYERS = 3;
constexpr std::size_t FOUR_PLAYERS = 4;

}  // namespace rtype::game::testing
