#pragma once

#include <cstddef>
#include <cstdint>
#include "BotSightTestConstants.hpp"

namespace rtype::tools::testing {

constexpr std::size_t FIRST_BOT_INDEX = 0;
constexpr std::size_t SECOND_BOT_INDEX = 1;
constexpr std::uint32_t HIGHER_ENEMY_ID = 8;

constexpr float OUT_OF_REACH_X = SHIP_X + 800.0F;
constexpr float NEARER_X = SHIP_X + 500.0F;
constexpr float ABOVE_THE_SHIP_Y = SHIP_Y - 100.0F;
constexpr float BELOW_THE_SHIP_Y = SHIP_Y + 100.0F;
constexpr float FAR_ABOVE_THE_SHIP_Y = SHIP_Y - 300.0F;

/** @brief Three ticks are 1/20 s, so 48 units in them are 960 per second: at
 * 300 units, the missile reaches a ship that stays in about 0.25 s, and a ship
 * that only flees left, at 480 per second, in about 0.45 s. */
constexpr float MISSILE_START_X = SHIP_X + 300.0F;
constexpr float MISSILE_NEXT_X = SHIP_X + 252.0F;

}  // namespace rtype::tools::testing
