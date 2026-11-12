#include "MovementSystem.hpp"
#include <span>
#include <vector>
#include "Bounds.hpp"
#include "BoundsOf.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Intent.hpp"
#include "Obstacle.hpp"
#include "ObstacleBlocking.hpp"
#include "ObstacleContact.hpp"
#include "Position.hpp"
#include "ShipControl.hpp"
#include "ShipMotion.hpp"
#include "SteadyMotion.hpp"
#include "SteadyTravel.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @brief Fills @p boxes with the box of every obstacle that blocks ships. */
void collectBlockingBoxes(engine::ComponentRegistry& components,
                          std::vector<engine::Bounds>& boxes) {
  boxes.clear();
  components.forEach<Obstacle, Position>(
      [&components, &boxes](engine::Entity body, const Obstacle& obstacle,
                            const Position& position) {
        const auto* const box = components.get<CollisionBox>(body);
        if (obstacle.contact == ObstacleContact::Blocking && box != nullptr) {
          boxes.push_back(boundsOf(position, *box));
        }
      });
}

/**
 * @brief Moves every entity with a Position, a ShipMotion and an Intent over
 * @p elapsed by the directions its Intent holds (see shipAfter()), stopped
 * against @p blocking and kept inside the play area.
 */
void moveByIntent(engine::ComponentRegistry& components,
                  engine::Duration elapsed,
                  std::span<const engine::Bounds> blocking) {
  components.forEach<Intent, ShipMotion>(
      [&components, elapsed, blocking](
          engine::Entity body, const Intent& intent, ShipMotion& motion) {
        auto* const position = components.get<Position>(body);
        if (position == nullptr) {
          return;
        }
        const MovingShip from{.position = *position, .motion = motion};
        const MovingShip moved = stoppedAtTheEdges(blockedByObstacles(
            from, shipAfter(from, heldDirectionsOf(intent.direction), elapsed),
            blocking));
        *position = moved.position;
        motion = moved.motion;
      });
}

/**
 * @brief Moves every entity with a Position, a SteadyMotion and an Intent over
 * @p elapsed, by its top speed times the share its Intent asks for on each
 * axis.
 */
void moveSteadily(engine::ComponentRegistry& components,
                  engine::Duration elapsed) {
  components.forEach<SteadyMotion, Intent>(
      [&components, elapsed](engine::Entity body, const SteadyMotion& motion,
                             const Intent& intent) {
        auto* const position = components.get<Position>(body);
        if (position == nullptr) {
          return;
        }
        const Position travel = steadyTravel(motion, intent.direction, elapsed);
        position->x += travel.x;
        position->y += travel.y;
      });
}

}  // namespace

void MovementSystem::update(engine::ComponentRegistry& components,
                            engine::Duration elapsed) {
  collectBlockingBoxes(components, blockingBoxes_);
  moveByIntent(components, elapsed, blockingBoxes_);
  moveSteadily(components, elapsed);
}

}  // namespace rtype::game
