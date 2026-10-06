#include <gtest/gtest.h>
#include "Entity.hpp"
#include "EntityRegistry.hpp"

/**
 * Given an empty registry
 * When two entities are created
 * Then they have different identifiers
 */
TEST(EntityRegistry, CreatedEntitiesAreDistinct) {
  rtype::engine::EntityRegistry registry;

  const rtype::engine::Entity first = registry.create();
  const rtype::engine::Entity second = registry.create();

  EXPECT_NE(first, second);
}

/**
 * Given an empty registry
 * When two entities are created
 * Then the registry holds two entities
 */
TEST(EntityRegistry, CountsCreatedEntities) {
  rtype::engine::EntityRegistry registry;

  registry.create();
  registry.create();

  EXPECT_EQ(registry.size(), 2U);
}

/**
 * Given an empty registry
 * When an entity is created
 * Then the registry reports it as alive
 */
TEST(EntityRegistry, CreatedEntityIsAlive) {
  rtype::engine::EntityRegistry registry;

  const rtype::engine::Entity entity = registry.create();

  EXPECT_TRUE(registry.isAlive(entity));
}

/**
 * Given a registry holding a single entity
 * When that entity is destroyed
 * Then the registry no longer reports it as alive
 */
TEST(EntityRegistry, DestroyedEntityIsNoLongerAlive) {
  rtype::engine::EntityRegistry registry;
  const rtype::engine::Entity entity = registry.create();

  registry.destroy(entity);

  EXPECT_FALSE(registry.isAlive(entity));
}

/**
 * Given a registry holding two entities
 * When one of them is destroyed
 * Then the registry holds one entity
 */
TEST(EntityRegistry, DestroyedEntityIsNoLongerCounted) {
  rtype::engine::EntityRegistry registry;
  const rtype::engine::Entity first = registry.create();
  registry.create();

  registry.destroy(first);

  EXPECT_EQ(registry.size(), 1U);
}

/**
 * Given a registry whose only entity was destroyed
 * When a new entity is created
 * Then it reuses the index with a different generation
 */
TEST(EntityRegistry, RecycledIndexCarriesNewGeneration) {
  rtype::engine::EntityRegistry registry;
  const rtype::engine::Entity destroyed = registry.create();
  registry.destroy(destroyed);

  const rtype::engine::Entity recycled = registry.create();

  EXPECT_EQ(recycled.index, destroyed.index);
  EXPECT_NE(recycled.generation, destroyed.generation);
}

/**
 * Given a destroyed entity whose index was recycled by a new entity
 * When the registry is asked whether the destroyed entity is alive
 * Then it answers no
 */
TEST(EntityRegistry, StaleEntityIsNotAliveAfterItsIndexIsRecycled) {
  rtype::engine::EntityRegistry registry;
  const rtype::engine::Entity stale = registry.create();
  registry.destroy(stale);
  registry.create();

  EXPECT_FALSE(registry.isAlive(stale));
}

/**
 * Given a destroyed entity whose index was recycled by a new entity
 * When the destroyed entity is destroyed again
 * Then the new entity stays alive
 */
TEST(EntityRegistry, DestroyingStaleEntityLeavesNewEntityAlive) {
  rtype::engine::EntityRegistry registry;
  const rtype::engine::Entity stale = registry.create();
  registry.destroy(stale);
  const rtype::engine::Entity recycled = registry.create();

  registry.destroy(stale);

  EXPECT_TRUE(registry.isAlive(recycled));
}

/**
 * Given a registry holding two entities
 * When the same entity is destroyed twice
 * Then the registry holds one entity
 */
TEST(EntityRegistry, DestroyingTwiceRemovesOneEntity) {
  rtype::engine::EntityRegistry registry;
  const rtype::engine::Entity first = registry.create();
  registry.create();

  registry.destroy(first);
  registry.destroy(first);

  EXPECT_EQ(registry.size(), 1U);
}

/**
 * Given an empty registry
 * When an entity that was never created is destroyed
 * Then the registry still holds no entity
 */
TEST(EntityRegistry, DestroyingUnknownEntityOnEmptyRegistryIsIgnored) {
  rtype::engine::EntityRegistry registry;

  registry.destroy(rtype::engine::Entity{.index = 0, .generation = 0});

  EXPECT_EQ(registry.size(), 0U);
}

/**
 * Given a registry holding an entity
 * When the entity at the index of that entity is requested
 * Then it is the entity that was created
 */
TEST(EntityRegistry, EntityAtGivesBackTheHandleOfAnAliveEntity) {
  rtype::engine::EntityRegistry registry;
  const rtype::engine::Entity entity = registry.create();

  EXPECT_EQ(registry.entityAt(entity.index), entity);
}

/**
 * Given an entity destroyed and its index recycled by a new entity
 * When the entity at that index is requested
 * Then it is the new entity, not the destroyed one
 */
TEST(EntityRegistry, EntityAtGivesTheNewHandleAfterTheIndexIsRecycled) {
  rtype::engine::EntityRegistry registry;
  const rtype::engine::Entity destroyed = registry.create();
  registry.destroy(destroyed);
  const rtype::engine::Entity recycled = registry.create();

  ASSERT_EQ(recycled.index, destroyed.index);
  EXPECT_EQ(registry.entityAt(destroyed.index), recycled);
  EXPECT_NE(registry.entityAt(destroyed.index), destroyed);
}
