#pragma once

namespace rtype::game {

/**
 * @brief Size of the entities the world gives a CollisionBox, in logical units:
 * the size the client draws each one at, its sprite on the entity sheet with
 * each pixel covering 4 logical units. A client test compares each size with
 * its sprite, so a new sprite comes with new sizes. The size of a ship is
 * SHIP_WIDTH and SHIP_HEIGHT, in ShipControlConstants.hpp.
 *
 * A missile of a player is 16 by 6 pixels of the sheet.
 */
constexpr float PLAYER_MISSILE_WIDTH = 64.0F;
constexpr float PLAYER_MISSILE_HEIGHT = 24.0F;

}  // namespace rtype::game
