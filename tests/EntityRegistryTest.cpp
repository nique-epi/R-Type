#include <gtest/gtest.h>
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
