#include "ObstacleBlocking.hpp"
#include <cmath>
#include <span>
#include "Bounds.hpp"
#include "CollisionConstants.hpp"
#include "Overlaps.hpp"
#include "Position.hpp"
#include "ShipControl.hpp"
#include "ShipControlConstants.hpp"

namespace rtype::game {

namespace {

/** @brief One axis of a ship and of an obstacle: their centers and their half
 * sizes along it. */
struct AxisSpan {
  float ship{};
  float shipHalf{};
  float obstacle{};
  float obstacleHalf{};
};

/** @returns The box of a ship whose center is at @p position. */
engine::Bounds shipBoundsAt(Position position) {
  return engine::Bounds{.centerX = position.x,
                        .centerY = position.y,
                        .width = SHIP_WIDTH,
                        .height = SHIP_HEIGHT};
}

/** @returns The ship and the obstacle along the horizontal axis. */
AxisSpan acrossOf(engine::Bounds ship, engine::Bounds obstacle) {
  return AxisSpan{.ship = ship.centerX,
                  .shipHalf = ship.width * engine::HALF,
                  .obstacle = obstacle.centerX,
                  .obstacleHalf = obstacle.width * engine::HALF};
}

/** @returns The ship and the obstacle along the vertical axis. */
AxisSpan downOf(engine::Bounds ship, engine::Bounds obstacle) {
  return AxisSpan{.ship = ship.centerY,
                  .shipHalf = ship.height * engine::HALF,
                  .obstacle = obstacle.centerY,
                  .obstacleHalf = obstacle.height * engine::HALF};
}

/** @returns Whether the two boxes of @p span share some length along it. */
bool overlapsAlong(AxisSpan span) {
  return std::abs(span.ship - span.obstacle) <
         span.shipHalf + span.obstacleHalf;
}

/** @returns How far the ship moves along @p span to touch the obstacle on the
 * side of it its center is on: below 0 towards the left or the top. */
float pushAlong(AxisSpan span) {
  return span.ship < span.obstacle
             ? (span.obstacle - span.obstacleHalf) - (span.ship + span.shipHalf)
             : (span.obstacle + span.obstacleHalf) -
                   (span.ship - span.shipHalf);
}

/** @returns Whether the ship, whose box is @p was before its step and @p is
 * after it, leaves @p obstacle along the horizontal axis. */
bool leavesAcross(engine::Bounds was, engine::Bounds is,
                  engine::Bounds obstacle) {
  const bool cameAcross = !overlapsAlong(acrossOf(was, obstacle));
  const bool cameDown = !overlapsAlong(downOf(was, obstacle));
  if (cameAcross != cameDown) {
    return cameAcross;
  }
  return std::abs(pushAlong(acrossOf(is, obstacle))) <=
         std::abs(pushAlong(downOf(is, obstacle)));
}

/** @returns @p ship moved out of @p obstacle, which it overlaps, as
 * blockedByObstacles() says, judged from its box before its step, @p was. */
MovingShip pushedOutOf(MovingShip ship, engine::Bounds was,
                       engine::Bounds obstacle) {
  const engine::Bounds is = shipBoundsAt(ship.position);
  if (leavesAcross(was, is, obstacle)) {
    ship.position.x += pushAlong(acrossOf(is, obstacle));
    ship.motion.velocityX = 0.0F;
  } else {
    ship.position.y += pushAlong(downOf(is, obstacle));
    ship.motion.velocityY = 0.0F;
  }
  return ship;
}

}  // namespace

MovingShip blockedByObstacles(MovingShip before, MovingShip after,
                              std::span<const engine::Bounds> obstacles) {
  const engine::Bounds was = shipBoundsAt(before.position);
  for (const engine::Bounds& obstacle : obstacles) {
    if (engine::overlaps(shipBoundsAt(after.position), obstacle)) {
      after = pushedOutOf(after, was, obstacle);
    }
  }
  return after;
}

}  // namespace rtype::game
