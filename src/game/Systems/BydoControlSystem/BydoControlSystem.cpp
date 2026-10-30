#include "BydoControlSystem.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "Intent.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

void BydoControlSystem::update(engine::ComponentRegistry& components,
                               engine::Duration /*elapsed*/) {
  components.forEach<EntityType, Intent>(
      [](engine::Entity, const EntityType& type, Intent& intent) {
        if (type == EntityType::Enemy) {
          intent.shooting = true;
        }
      });
}

}  // namespace rtype::game
