#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;

namespace {

struct Position {
  float x;
  float y;
};

struct Velocity {
  float x;
  float y;
};

constexpr float FIRST_X = 1.0F;
constexpr float FIRST_Y = 2.0F;
constexpr float SECOND_X = 3.0F;
constexpr float SECOND_Y = 4.0F;
constexpr float VELOCITY_X = 0.5F;
constexpr float VELOCITY_Y = 0.25F;
constexpr std::size_t ENTITY_COUNT = 10;

}  // namespace

/**
 * Given an entity with a Position and a Velocity, one with a Position only
 * and one with a Velocity only
 * When every entity having a Position and a Velocity is visited
 * Then only the first entity is visited
 */
TEST(ComponentQuery, VisitsOnlyEntitiesHavingEveryRequestedComponent) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity both = entities.create();
  const Entity positionOnly = entities.create();
  const Entity velocityOnly = entities.create();
  components.add<Position>(both, {.x = FIRST_X, .y = FIRST_Y});
  components.add<Velocity>(both, {.x = VELOCITY_X, .y = VELOCITY_Y});
  components.add<Position>(positionOnly, {.x = SECOND_X, .y = SECOND_Y});
  components.add<Velocity>(velocityOnly, {.x = VELOCITY_X, .y = VELOCITY_Y});
  std::vector<Entity> visited;

  components.forEach<Position, Velocity>(
      [&visited](Entity entity, Position&, Velocity&) {
        visited.push_back(entity);
      });

  EXPECT_EQ(visited, std::vector<Entity>{both});
}

/**
 * Given two entities with different Positions, both with a Velocity
 * When every entity having a Position and a Velocity is visited
 * Then each entity receives its own Position
 */
TEST(ComponentQuery, GivesTheComponentsOfTheVisitedEntity) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity first = entities.create();
  const Entity second = entities.create();
  components.add<Position>(first, {.x = FIRST_X, .y = FIRST_Y});
  components.add<Position>(second, {.x = SECOND_X, .y = SECOND_Y});
  components.add<Velocity>(first, {.x = VELOCITY_X, .y = VELOCITY_Y});
  components.add<Velocity>(second, {.x = VELOCITY_X, .y = VELOCITY_Y});
  std::map<std::uint32_t, float> positionXByEntityIndex;

  components.forEach<Position, Velocity>(
      [&positionXByEntityIndex](Entity entity, Position& position, Velocity&) {
        positionXByEntityIndex[entity.index] = position.x;
      });

  const std::map<std::uint32_t, float> expected{{first.index, FIRST_X},
                                                {second.index, SECOND_X}};
  EXPECT_EQ(positionXByEntityIndex, expected);
}

/**
 * Given an entity with a Position and a Velocity
 * When the visit adds the Velocity to the Position
 * Then the Position stored for the entity holds the sum
 */
TEST(ComponentQuery, CallbackModifiesComponentsInPlace) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = entities.create();
  components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});
  components.add<Velocity>(entity, {.x = VELOCITY_X, .y = VELOCITY_Y});

  components.forEach<Position, Velocity>(
      [](Entity, Position& position, Velocity& velocity) {
        position.x += velocity.x;
        position.y += velocity.y;
      });

  ASSERT_NE(components.get<Position>(entity), nullptr);
  EXPECT_EQ(components.get<Position>(entity)->x, FIRST_X + VELOCITY_X);
  EXPECT_EQ(components.get<Position>(entity)->y, FIRST_Y + VELOCITY_Y);
}

/**
 * Given entities with a Position and no Velocity ever added
 * When every entity having a Position and a Velocity is visited
 * Then nothing is visited
 */
TEST(ComponentQuery, VisitsNothingWhenATypeWasNeverAdded) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  components.add<Position>(entities.create(), {.x = FIRST_X, .y = FIRST_Y});
  std::size_t visits = 0;

  components.forEach<Position, Velocity>(
      [&visits](Entity, Position&, Velocity&) { ++visits; });

  EXPECT_EQ(visits, 0U);
}

/**
 * Given entities with a Position and a Velocity, two of which then lose their
 * Position
 * When every entity having a Position and a Velocity is visited
 * Then each remaining entity is visited once and the two others never
 */
TEST(ComponentQuery, VisitsEachRemainingEntityOnceAfterRemovals) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  std::vector<Entity> created;
  for (std::size_t count = 0; count < ENTITY_COUNT; ++count) {
    const Entity entity = entities.create();
    components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});
    components.add<Velocity>(entity, {.x = VELOCITY_X, .y = VELOCITY_Y});
    created.push_back(entity);
  }
  const std::size_t firstRemoved = 2;
  const std::size_t secondRemoved = 5;
  components.remove<Position>(created[firstRemoved]);
  components.remove<Position>(created[secondRemoved]);
  std::map<std::uint32_t, int> visitsByEntityIndex;

  components.forEach<Position, Velocity>(
      [&visitsByEntityIndex](Entity entity, Position&, Velocity&) {
        ++visitsByEntityIndex[entity.index];
      });

  std::map<std::uint32_t, int> expected;
  for (std::size_t position = 0; position < ENTITY_COUNT; ++position) {
    if (position != firstRemoved && position != secondRemoved) {
      expected[created[position].index] = 1;
    }
  }
  EXPECT_EQ(visitsByEntityIndex, expected);
}
