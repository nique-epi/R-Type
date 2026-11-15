#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::game::testing {

/** @brief The boss of the arrival tests: network id 61, of the sheet
 * number 7; it appears with SHEET_BOSS_HEALTH points. */
constexpr std::uint32_t ARRIVING_BOSS_ID = 61;
constexpr std::uint8_t ARRIVING_BOSS_SHEET_NUMBER = 7;

/** @brief The players are warned of a boss 4400 ms before anything steers
 * it, which is 264 ticks of a game at 60 ticks a second. */
constexpr std::uint32_t MILLISECONDS_OF_A_BOSS_WARNING = 4400;
constexpr std::size_t TICKS_OF_A_BOSS_WARNING = 264;

}  // namespace rtype::game::testing
