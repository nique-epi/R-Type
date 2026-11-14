#include "MovementSystem.hpp"
#include <span>
#include "Bounds.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Intent.hpp"
#include "ObstacleBlocking.hpp"
#include "Position.hpp"
#include "ShipControl.hpp"
#include "ShipMotion.hpp"
#include "SteadyMotion.hpp"
#include "SteadyTravel.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/**
 * @brief Moves every entity with a Position, a ShipMotion and an Intent over
 * @p elapsed by the directions its Intent holds (see shipAfter()), stopped
 * against @p blocking (see blockedByObstacles()).
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
        const MovingShip moved = blockedByObstacles(
            from, shipAfter(from, heldDirectionsOf(intent.direction), elapsed),
            blocking);
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
