#pragma once

#include <span>
#include "Bounds.hpp"
#include "ShipControl.hpp"

namespace rtype::game {

/**
 * @returns @p after moved out of each box of @p obstacles it overlaps, in
 * their order, along one axis: the axis it came across, judged from where it
 * was, @p before; or the axis it is least deep along, when it came across both
 * axes or neither (a wall that moved onto it). It loses its velocity along that
 * axis, as against an edge of the play area. A ship that only touches a box
 * along an edge is not moved (engine::overlaps()). The ship is the box
 * SHIP_WIDTH by SHIP_HEIGHT on its center, as the sheet of a ship must give
 * it.
 *
 * The server runs it after shipAfter() for each ship, against the blocking
 * obstacles of the game; the client runs it the same way to predict its own
 * ship, against the blocking obstacles it draws.
 */
[[nodiscard]] MovingShip blockedByObstacles(
    MovingShip before, MovingShip after,
    std::span<const engine::Bounds> obstacles);

}  // namespace rtype::game
