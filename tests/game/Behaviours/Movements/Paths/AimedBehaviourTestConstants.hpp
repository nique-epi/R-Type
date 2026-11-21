#pragma once

#include "Position.hpp"

namespace rtype::game::testing {

/** @brief Where an aimed shot is on its first tick. */
constexpr Position WHERE_THE_SHOT_STARTS{.x = 1000.0F, .y = 500.0F};

/** @brief A ship 300 units left of the shot and 400 below it, 500 away: the
 * shot heads 0.6 of its speed left and 0.8 down. */
constexpr Position SHIP_DOWN_LEFT_OF_THE_SHOT{.x = 700.0F, .y = 900.0F};
constexpr float AIMED_ACROSS = -0.6F;
constexpr float AIMED_DOWN = 0.8F;

/** @brief Where that ship has flown by the next tick: up, still left of the
 * shot. */
constexpr Position SHIP_UP_LEFT_OF_THE_SHOT{.x = 700.0F, .y = 100.0F};

/** @brief A ship right of the shot, behind an enemy shot that flies left. */
constexpr Position SHIP_BEHIND_THE_SHOT{.x = 1200.0F, .y = 300.0F};

}  // namespace rtype::game::testing
