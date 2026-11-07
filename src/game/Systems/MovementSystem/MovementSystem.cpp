#include "MovementSystem.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Intent.hpp"
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
 * @p elapsed by the directions its Intent holds (see shipAfter()).
 */
void moveByIntent(engine::ComponentRegistry& components,
                  engine::Duration elapsed) {
  components.forEach<Intent, ShipMotion>(
      [&components, elapsed](engine::Entity body, const Intent& intent,
                             ShipMotion& motion) {
        auto* const position = components.get<Position>(body);
        if (position == nullptr) {
          return;
        }
        const MovingShip moved =
            shipAfter(MovingShip{.position = *position, .motion = motion},
                      heldDirectionsOf(intent.direction), elapsed);
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
  moveByIntent(components, elapsed);
  moveSteadily(components, elapsed);
}

}  // namespace rtype::game
