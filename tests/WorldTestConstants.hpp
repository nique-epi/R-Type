#pragma once

#include <cstdint>

namespace rtype::game::testing {

/**
 * @brief Where the world tests spawn their entities, in logical units: some
 * place, and another one far enough to tell two entities apart.
 */
constexpr float SOME_X = 3.0F;
constexpr float SOME_Y = 4.0F;
constexpr float OTHER_X = 30.0F;
constexpr float OTHER_Y = 40.0F;

/** @brief The health a new Bydo has, as the world tests expect it. */
constexpr int THREE_HEALTH_POINTS = 3;

/** @brief The player whose missile the world tests create. */
constexpr std::uint8_t SECOND_PLAYER_NUMBER = 2;

/**
 * @brief How fast a missile of a player flies to the right, as the world tests
 * expect it: 960 logical units per second, twice the speed of a ship.
 */
constexpr float MISSILE_UNITS_PER_SECOND = 960.0F;

/**
 * @brief How fast a missile of an enemy flies to the left, as the world tests
 * expect it: 360 logical units per second, three quarters of the speed of a
 * ship.
 */
constexpr float ENEMY_MISSILE_UNITS_PER_SECOND = 360.0F;

}  // namespace rtype::game::testing
