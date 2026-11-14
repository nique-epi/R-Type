#pragma once

#include <span>
#include <vector>
#include "Bounds.hpp"
#include "ShipControl.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @returns @p after moved out of each box of @p obstacles it overlaps, in
 * their order, along one axis: the axis it came across, judged from where it
 * was, @p before; or the axis it is least deep along, when it came across both
 * axes or neither (a wall that moved onto it). It loses its velocity along that
 * axis, as against an edge of the play area. A ship that only touches a box
 * along an edge is not moved (engine::overlaps()). The ship is then kept
 * inside the play area (stoppedAtTheEdges()): a ship a wall pushes past an
 * edge stays over the wall, which CollisionRules turns into a contact. The ship
 * is the box SHIP_WIDTH by SHIP_HEIGHT on its center, as the sheet of a ship
 * must give it.
 *
 * The server runs it on what shipAfter() gives for each ship, against the
 * blocking obstacles of the game; the client runs it the same way to predict
 * its own ship, against the blocking obstacles it draws.
 */
[[nodiscard]] MovingShip blockedByObstacles(
    MovingShip before, MovingShip after,
    std::span<const engine::Bounds> obstacles);

/**
 * @brief Fills @p boxes with the boxes of every Obstacle of @p components whose
 * contact is blocking, each box of its HitboxParts when it has some: what
 * blockedByObstacles() stops a ship against, on the server and in the
 * prediction of the client alike. Not during a forEach of @p components.
 */
void collectBlockingBoxes(engine::ComponentRegistry& components,
                          std::vector<engine::Bounds>& boxes);

}  // namespace rtype::game
