#include "ObstacleBlocking.hpp"
#include <cmath>
#include <span>
#include <vector>
#include "Bounds.hpp"
#include "BoundsOf.hpp"
#include "CollidingBoxes.hpp"
#include "CollisionBox.hpp"
#include "CollisionConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "HitboxParts.hpp"
#include "Obstacle.hpp"
#include "ObstacleContact.hpp"
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

/** @returns Whether the ship, whose box was @p was before its step, leaves
 * @p obstacle along the horizontal axis, @p push being how far it moves along
 * each axis to leave it. */
bool leavesAcross(engine::Bounds was, engine::Bounds obstacle, Position push) {
  const bool cameAcross = !overlapsAlong(acrossOf(was, obstacle));
  const bool cameDown = !overlapsAlong(downOf(was, obstacle));
  if (cameAcross != cameDown) {
    return cameAcross;
  }
  return std::abs(push.x) <= std::abs(push.y);
}

/** @returns @p ship moved out of @p obstacle, which it overlaps, as
 * blockedByObstacles() says, judged from its box before its step, @p was. */
MovingShip pushedOutOf(MovingShip ship, engine::Bounds was,
                       engine::Bounds obstacle) {
  const engine::Bounds is = shipBoundsAt(ship.position);
  const Position push{.x = pushAlong(acrossOf(is, obstacle)),
                      .y = pushAlong(downOf(is, obstacle))};
  if (leavesAcross(was, obstacle, push)) {
    ship.position.x += push.x;
    ship.motion.velocityX = 0.0F;
  } else {
    ship.position.y += push.y;
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
  return stoppedAtTheEdges(after);
}

void collectBlockingBoxes(engine::ComponentRegistry& components,
                          std::vector<engine::Bounds>& boxes) {
  boxes.clear();
  components.forEach<Obstacle, Position>(
      [&components, &boxes](engine::Entity body, const Obstacle& obstacle,
                            const Position& position) {
        const auto* const box = components.get<CollisionBox>(body);
        if (obstacle.contact != ObstacleContact::Blocking || box == nullptr) {
          return;
        }
        for (const CollisionBox& part :
             collidingBoxesOf(*box, components.get<HitboxParts>(body))) {
          boxes.push_back(boundsOf(position, part));
        }
      });
}

}  // namespace rtype::game
