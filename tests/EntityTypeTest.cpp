#include <gtest/gtest.h>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::EntityType;

/**
 * Given a player ship, an enemy and a player missile, each with its EntityType
 * When the entities whose type is Enemy are collected
 * Then only the enemy is found
 */
TEST(EntityType, EntitiesAreSelectedByTheirType) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity player = entities.create();
  const Entity enemy = entities.create();
  const Entity missile = entities.create();
  components.add(player, EntityType::Player);
  components.add(enemy, EntityType::Enemy);
  components.add(missile, EntityType::PlayerMissile);
  std::vector<Entity> enemies;

  components.forEach<EntityType, EntityType>(
      [&enemies](Entity entity, const EntityType& type, const EntityType&) {
        if (type == EntityType::Enemy) {
          enemies.push_back(entity);
        }
      });

  EXPECT_EQ(enemies, std::vector<Entity>{enemy});
}
