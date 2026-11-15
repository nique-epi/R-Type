#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::game::testing {

/** @brief The boss of the arrival tests: network id 61, of the sheet
 * number 7, with the 240 points of its sheet. */
constexpr std::uint32_t ARRIVING_BOSS_ID = 61;
constexpr std::uint8_t ARRIVING_BOSS_SHEET_NUMBER = 7;
constexpr int ARRIVING_BOSS_HEALTH = 240;

/** @brief The players are warned of a boss 4400 ms before anything steers
 * it, which is 264 ticks of a game at 60 ticks a second. */
constexpr std::uint32_t MILLISECONDS_OF_A_BOSS_WARNING = 4400;
constexpr std::size_t TICKS_OF_A_BOSS_WARNING = 264;

/** @brief Each player past the first adds a quarter: 360 points for three
 * players. A difficulty at 130 percent gives 312 points to one player. */
constexpr std::size_t THREE_CONNECTED_PILOTS = 3;
constexpr int ARRIVING_BOSS_HEALTH_FOR_THREE = 360;
constexpr int HARD_DIFFICULTY_HEALTH_PERCENT = 130;
constexpr int ARRIVING_BOSS_HEALTH_WHEN_HARD = 312;

}  // namespace rtype::game::testing
