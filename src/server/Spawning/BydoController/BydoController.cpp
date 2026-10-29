#include "BydoController.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "Intent.hpp"
#include "World.hpp"

namespace rtype::server {

void steerBydos(game::World& world) {
  engine::ComponentRegistry& components = world.components();
  components.forEach<game::EntityType, game::Intent>(
      [](engine::Entity /*body*/, const game::EntityType& type,
         game::Intent& intent) {
        if (type == game::EntityType::Enemy) {
          intent.shooting = true;
        }
      });
}

}  // namespace rtype::server
