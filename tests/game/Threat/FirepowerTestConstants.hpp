#pragma once

#include <cstddef>
#include <cstdint>
#include "ShotPattern.hpp"

namespace rtype::game::testing {

/** @brief The ticks between two shots of a gun, and half of it: a gun that
 * fires twice as often fires twice as much. */
constexpr std::size_t GUN_INTERVAL_TICKS = 20;
constexpr std::size_t HALF_GUN_INTERVAL_TICKS = 10;

/** @brief One projectile straight ahead, as the gun of a sheet fires. */
constexpr ShotPattern ONE_STRAIGHT_PROJECTILE{};

/** @brief Three projectiles in rows, all flying ahead. */
constexpr std::uint8_t THREE_PROJECTILES = 3;
constexpr float ROWS_APART = 24.0F;
constexpr ShotPattern THREE_STRAIGHT_PROJECTILES{
    .count = THREE_PROJECTILES, .spreadDegrees = 0.0F, .stepY = ROWS_APART};

/** @brief Three projectiles over a fan of 160 degrees: one ahead, one 80
 * degrees up and one 80 degrees down, wider than the cone that counts. */
constexpr float WIDE_FAN_DEGREES = 160.0F;
constexpr ShotPattern WIDE_FAN_OF_THREE{.count = THREE_PROJECTILES,
                                        .spreadDegrees = WIDE_FAN_DEGREES};

/** @brief Five projectiles over the same fan: at 80 and 40 degrees on each
 * side and ahead; the three within 45 degrees count. */
constexpr std::uint8_t FIVE_PROJECTILES = 5;
constexpr ShotPattern WIDE_FAN_OF_FIVE{.count = FIVE_PROJECTILES,
                                       .spreadDegrees = WIDE_FAN_DEGREES};

/** @brief A gun of a sheet that fires all its projectiles wider than the
 * cone: two at 80 degrees up and down. */
constexpr std::uint8_t TWO_PROJECTILES = 2;
constexpr ShotPattern SIDEWAYS_PAIR{.count = TWO_PROJECTILES,
                                    .spreadDegrees = WIDE_FAN_DEGREES};

/** @brief The firepower of a gun, in percent of the gun of its sheet. */
constexpr int AS_MUCH_AS_THE_SHEET = 100;
constexpr int TWICE_THE_SHEET = 200;
constexpr int THREE_TIMES_THE_SHEET = 300;

/** @brief The strongest level of a weapon of the shipped ship: five
 * projectiles ahead every 20 ticks for red, three every 12 ticks for blue,
 * against one every 20 ticks for its gun. */
constexpr int STRONGEST_SHIPPED_WEAPON = 500;

/** @brief The fifth level of red on a ship, against the gun of the other
 * ship: their mean is 300 %. */
constexpr float RED_FAN_DEGREES = 48.0F;
constexpr ShotPattern RED_SECOND_LEVEL{.count = FIVE_PROJECTILES,
                                       .spreadDegrees = RED_FAN_DEGREES};
constexpr int MEAN_OF_THE_GUN_AND_THE_STRONGEST = 300;

/** @brief The players two ships of a test are given to, and where the
 * ships stand. */
constexpr std::uint8_t FIRST_PLAYER = 0;
constexpr std::uint8_t SECOND_PLAYER = 1;
constexpr float FIRST_SHIP_X = 200.0F;
constexpr float SECOND_SHIP_X = 400.0F;
constexpr float SHIPS_Y = 540.0F;

}  // namespace rtype::game::testing
