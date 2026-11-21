#include "EnemyCount.hpp"
#include <cstddef>
#include "Boss.hpp"
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

std::size_t bossesIn(engine::ComponentRegistry& components) {
  std::size_t bosses = 0;
  components.forEach<Boss, EntityType>(
      [&bosses](engine::Entity, const Boss&, const EntityType&) { ++bosses; });
  return bosses;
}

}  // namespace rtype::game
