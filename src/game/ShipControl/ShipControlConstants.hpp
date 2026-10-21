#pragma once

namespace rtype::game {

/**
 * @brief How far a ship moves in one second, in logical units, along a straight
 * line or a diagonal alike.
 *
 * Provisional, until the game is tuned: at 480 units per second a ship crosses
 * the width of the playfield in 4 seconds and its height in 2.25 seconds, two
 * pixels of a sprite per tick at 60 ticks per second.
 */
constexpr float SHIP_SPEED = 480.0F;

/**
 * @brief Size of a ship, in logical units: the size the client draws it at, 32
 * by 16 pixels of the entity sheet, each covering 4 logical units. A ship stays
 * whole inside the playfield, its center half this size away from every edge.
 */
constexpr float SHIP_WIDTH = 128.0F;
constexpr float SHIP_HEIGHT = 64.0F;

}  // namespace rtype::game
