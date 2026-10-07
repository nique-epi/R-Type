#include "MovementSystem.hpp"
#include <chrono>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"
#include "Velocity.hpp"

namespace rtype::game {

void MovementSystem::update(engine::ComponentRegistry& components,
                            engine::Duration elapsed) {
  const float elapsedSeconds = std::chrono::duration<float>(elapsed).count();
  components.forEach<Position, Velocity>(
      [elapsedSeconds](engine::Entity, Position& position,
                       const Velocity& velocity) {
        position.x += velocity.x * elapsedSeconds;
        position.y += velocity.y * elapsedSeconds;
      });
}

}  // namespace rtype::game
