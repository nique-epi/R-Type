#pragma once

#include "Position.hpp"

namespace rtype::game::testing {

/** @brief A Bydo somewhere in the play area. */
constexpr Position SOME_BYDO{.x = 1000.0F, .y = 300.0F};

/**
 * @brief Where a missile of SOME_BYDO starts: the left side of the Bydo is half
 * of its 96 units before its center, at 952, and the center of the missile is
 * half of its 32 units further, at 936.
 */
constexpr float ENEMY_MISSILE_START_X = 936.0F;

}  // namespace rtype::game::testing
