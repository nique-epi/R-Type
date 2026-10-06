#include <gtest/gtest.h>
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::DeadEntityException;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;

namespace {

struct alignas(2 * sizeof(float)) Position {
  float x;
  float y;
};

struct Health {
  int points;
};

constexpr float FIRST_X = 1.0F;
constexpr float FIRST_Y = 2.0F;
constexpr float SECOND_X = 3.0F;
constexpr float SECOND_Y = 4.0F;
constexpr float THIRD_X = 5.0F;
constexpr float THIRD_Y = 6.0F;
constexpr int FULL_HEALTH = 100;
constexpr int LOW_HEALTH = 10;
constexpr std::size_t MANY_ENTITIES = 1000;

}  // namespace

/**
 * Given an entity with no component
 * When a Position is added
 * Then the Position can be read back and is reported present
 */
TEST(ComponentRegistry, AddedComponentIsReadable) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = entities.create();

  components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});

  ASSERT_TRUE(components.has<Position>(entity));
  const Position* position = components.get<Position>(entity);
  ASSERT_NE(position, nullptr);
  EXPECT_EQ(position->x, FIRST_X);
  EXPECT_EQ(position->y, FIRST_Y);
}

/**
 * Given an entity with no component
 * When a Position is looked up
 * Then it is absent
 */
TEST(ComponentRegistry, ComponentIsAbsentBeforeItIsAdded) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = entities.create();

  EXPECT_FALSE(components.has<Position>(entity));
  EXPECT_EQ(components.get<Position>(entity), nullptr);
}

/**
 * Given an entity that already has a Position
 * When another Position is added
 * Then the first one is replaced
 */
TEST(ComponentRegistry, AddingSameTypeReplacesTheValue) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = entities.create();
  components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});

  components.add<Position>(entity, {.x = SECOND_X, .y = SECOND_Y});

  const Position* position = components.get<Position>(entity);
  ASSERT_NE(position, nullptr);
  EXPECT_EQ(position->x, SECOND_X);
  EXPECT_EQ(position->y, SECOND_Y);
}

/**
 * Given an entity with a Position
 * When a Health is added then removed
 * Then the Position is untouched and the Health is gone
 */
TEST(ComponentRegistry, ComponentTypesAreIndependent) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = entities.create();
  components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});

  components.add<Health>(entity, {.points = FULL_HEALTH});
  components.remove<Health>(entity);

  EXPECT_TRUE(components.has<Position>(entity));
  EXPECT_FALSE(components.has<Health>(entity));
}

/**
 * Given an entity with a Position
 * When the Position is removed twice
 * Then the first removal reports success and the second reports nothing removed
 */
TEST(ComponentRegistry, RemoveReportsWhetherAnythingWasRemoved) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = entities.create();
  components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});

  EXPECT_TRUE(components.remove<Position>(entity));
  EXPECT_FALSE(components.remove<Position>(entity));
  EXPECT_FALSE(components.has<Position>(entity));
}

/**
 * Given three entities with a Position each
 * When the first entity's Position is removed
 * Then the other two keep their own values
 */
TEST(ComponentRegistry, RemovingOneComponentLeavesOthersIntact) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity first = entities.create();
  const Entity second = entities.create();
  const Entity third = entities.create();
  components.add<Position>(first, {.x = FIRST_X, .y = FIRST_Y});
  components.add<Position>(second, {.x = SECOND_X, .y = SECOND_Y});
  components.add<Position>(third, {.x = THIRD_X, .y = THIRD_Y});

  components.remove<Position>(first);

  EXPECT_FALSE(components.has<Position>(first));
  ASSERT_NE(components.get<Position>(second), nullptr);
  EXPECT_EQ(components.get<Position>(second)->x, SECOND_X);
  ASSERT_NE(components.get<Position>(third), nullptr);
  EXPECT_EQ(components.get<Position>(third)->x, THIRD_X);
}

/**
 * Given two entities with a Position each
 * When the last one added is removed
 * Then the other keeps its value
 */
TEST(ComponentRegistry, RemovingLastComponentLeavesOtherIntact) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity first = entities.create();
  const Entity second = entities.create();
  components.add<Position>(first, {.x = FIRST_X, .y = FIRST_Y});
  components.add<Position>(second, {.x = SECOND_X, .y = SECOND_Y});

  components.remove<Position>(second);

  EXPECT_FALSE(components.has<Position>(second));
  ASSERT_NE(components.get<Position>(first), nullptr);
  EXPECT_EQ(components.get<Position>(first)->x, FIRST_X);
}

/**
 * Given an entity with a Position and a Health
 * When the entity is destroyed and its index is recycled
 * Then the new entity carries none of the old components
 */
TEST(ComponentRegistry, DestroyingAnEntityRemovesAllItsComponents) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity destroyed = entities.create();
  components.add<Position>(destroyed, {.x = FIRST_X, .y = FIRST_Y});
  components.add<Health>(destroyed, {.points = FULL_HEALTH});

  components.destroy(destroyed);
  const Entity recycled = entities.create();

  ASSERT_EQ(recycled.index, destroyed.index);
  EXPECT_FALSE(components.has<Position>(recycled));
  EXPECT_FALSE(components.has<Health>(recycled));
  EXPECT_FALSE(entities.isAlive(destroyed));
}

/**
 * Given a destroyed entity
 * When its components are read, removed or added through the stale handle
 * Then reads find nothing, removal does nothing and adding throws
 */
TEST(ComponentRegistry, StaleHandleReachesNoComponent) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity stale = entities.create();
  components.add<Position>(stale, {.x = FIRST_X, .y = FIRST_Y});
  components.destroy(stale);
  entities.create();

  EXPECT_FALSE(components.has<Position>(stale));
  EXPECT_EQ(components.get<Position>(stale), nullptr);
  EXPECT_FALSE(components.remove<Position>(stale));
  EXPECT_THROW(components.add<Position>(stale, {.x = FIRST_X, .y = FIRST_Y}),
               DeadEntityException);
}

/**
 * Given an entity destroyed and its index recycled by a new entity with a
 * Health When the old entity is destroyed again through its stale handle Then
 * the new entity keeps its Health
 */
TEST(ComponentRegistry, DestroyingThroughStaleHandleSparesRecycledEntity) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity stale = entities.create();
  components.destroy(stale);
  const Entity recycled = entities.create();
  components.add<Health>(recycled, {.points = LOW_HEALTH});

  components.destroy(stale);

  ASSERT_NE(components.get<Health>(recycled), nullptr);
  EXPECT_EQ(components.get<Health>(recycled)->points, LOW_HEALTH);
  EXPECT_TRUE(entities.isAlive(recycled));
}

/**
 * Given many entities
 * When the last one receives a component
 * Then it is readable and the others have none
 */
TEST(ComponentRegistry, ComponentOnHighEntityIndexIsReadable) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  Entity last = entities.create();
  for (std::size_t created = 1; created < MANY_ENTITIES; ++created) {
    last = entities.create();
  }

  components.add<Health>(last, {.points = FULL_HEALTH});

  ASSERT_NE(components.get<Health>(last), nullptr);
  EXPECT_EQ(components.get<Health>(last)->points, FULL_HEALTH);
  EXPECT_FALSE(components.has<Health>(Entity{.index = 0, .generation = 0}));
}
