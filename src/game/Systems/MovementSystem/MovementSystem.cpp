#include "MovementSystem.hpp"
#include <algorithm>
#include <chrono>
#include "ComponentRegistry.hpp"
#include "DirectionConstants.hpp"
#include "Entity.hpp"
#include "Intent.hpp"
#include "Position.hpp"
#include "ShipControl.hpp"
#include "ShipMotion.hpp"
#include "SteadyMotion.hpp"
#include "TimeConstants.hpp"
#include "Velocity.hpp"

namespace rtype::game {

namespace {

/**
 * @brief Moves every entity with a Position and a Velocity by its velocity
 * over @p elapsed.
 */
void moveByVelocity(engine::ComponentRegistry& components,
                    engine::Duration elapsed) {
  const float elapsedSeconds = std::chrono::duration<float>(elapsed).count();
  components.forEach<Position, Velocity>(
      [elapsedSeconds](engine::Entity, Position& position,
                       const Velocity& velocity) {
        position.x += velocity.x * elapsedSeconds;
        position.y += velocity.y * elapsedSeconds;
      });
}

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

/** @returns @p share brought within -FULL_STEERING and FULL_STEERING. */
float clampedShare(float share) {
  return std::clamp(share, -FULL_STEERING, FULL_STEERING);
}

/**
 * @brief Moves every entity with a Position, a SteadyMotion and an Intent over
 * @p elapsed, by its top speed times the share its Intent asks for on each
 * axis.
 */
void moveSteadily(engine::ComponentRegistry& components,
                  engine::Duration elapsed) {
  const float elapsedSeconds = std::chrono::duration<float>(elapsed).count();
  components.forEach<SteadyMotion, Intent>(
      [&components, elapsedSeconds](engine::Entity body,
                                    const SteadyMotion& motion,
                                    const Intent& intent) {
        auto* const position = components.get<Position>(body);
        if (position == nullptr) {
          return;
        }
        const float reach = motion.topSpeed * elapsedSeconds;
        position->x += clampedShare(intent.direction.x) * reach;
        position->y += clampedShare(intent.direction.y) * reach;
      });
}

}  // namespace

void MovementSystem::update(engine::ComponentRegistry& components,
                            engine::Duration elapsed) {
  moveByVelocity(components, elapsed);
  moveByIntent(components, elapsed);
  moveSteadily(components, elapsed);
}

}  // namespace rtype::game
