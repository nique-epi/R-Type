#pragma once

namespace rtype::game {

/**
 * @brief How fast a missile of a player flies to the right, in logical units
 * per second.
 *
 * Provisional, until the game is tuned: twice SHIP_SPEED, so a ship never
 * catches up with its own missiles. A missile crosses the width of the
 * playfield in 2 seconds, 16 units per tick at 60 ticks per second.
 */
constexpr float PLAYER_MISSILE_SPEED = 960.0F;

/**
 * @brief How fast a missile of an enemy flies to the left, in logical units per
 * second.
 *
 * Provisional, until the game is tuned: three quarters of SHIP_SPEED, so a ship
 * outruns it. A missile crosses the width of the playfield in 5.3 seconds, 6
 * units per tick at 60 ticks per second.
 */
constexpr float ENEMY_MISSILE_SPEED = 360.0F;

}  // namespace rtype::game
