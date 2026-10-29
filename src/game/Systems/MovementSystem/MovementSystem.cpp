#include "MovementSystem.hpp"
#include <chrono>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Intent.hpp"
#include "Position.hpp"
#include "ShipControl.hpp"
#include "ShipMotion.hpp"
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
        Position* const position = components.get<Position>(body);
        if (position == nullptr) {
          return;
        }
        const MovingShip moved =
            shipAfter(MovingShip{.position = *position, .motion = motion},
                      intent.directions, elapsed);
        *position = moved.position;
        motion = moved.motion;
      });
}

}  // namespace

void MovementSystem::update(engine::ComponentRegistry& components,
                            engine::Duration elapsed) {
  moveByVelocity(components, elapsed);
  moveByIntent(components, elapsed);
}

}  // namespace rtype::game
