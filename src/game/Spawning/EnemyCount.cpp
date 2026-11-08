#include "EnemyCount.hpp"
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"

namespace rtype::game {

std::size_t enemiesIn(engine::ComponentRegistry& components) {
  std::size_t enemies = 0;
  components.forEach<EntityType, EntityType>(
      [&enemies](engine::Entity, const EntityType& type, const EntityType&) {
        if (type == EntityType::Enemy) {
          ++enemies;
        }
      });
  return enemies;
}

}  // namespace rtype::game
