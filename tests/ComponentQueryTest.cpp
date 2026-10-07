#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <map>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"

using rtype::engine::ComponentChangeDuringIterationException;
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

struct Health {
  int points;
};

struct CallbackFailure : std::exception {};

constexpr float FIRST_X = 1.0F;
constexpr float FIRST_Y = 2.0F;
constexpr float SECOND_X = 3.0F;
constexpr float SECOND_Y = 4.0F;
constexpr float VELOCITY_X = 0.5F;
constexpr float VELOCITY_Y = 0.25F;
constexpr std::size_t ENTITY_COUNT = 10;
constexpr int FULL_HEALTH = 100;
constexpr int LOW_HEALTH = 10;

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

/**
 * Given entities with a Position and a Velocity
 * When every visited entity is destroyed during the visit
 * Then every entity is visited and none is alive afterwards
 */
TEST(ComponentQuery, DestroyingEveryVisitedEntityVisitsThemAll) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  std::vector<Entity> created;
  for (std::size_t count = 0; count < ENTITY_COUNT; ++count) {
    const Entity entity = entities.create();
    components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});
    components.add<Velocity>(entity, {.x = VELOCITY_X, .y = VELOCITY_Y});
    created.push_back(entity);
  }
  std::size_t visits = 0;

  components.forEach<Position, Velocity>(
      [&components, &visits](Entity entity, Position&, Velocity&) {
        ++visits;
        components.destroy(entity);
      });

  EXPECT_EQ(visits, ENTITY_COUNT);
  EXPECT_EQ(entities.size(), 0U);
  for (const Entity& entity : created) {
    EXPECT_FALSE(entities.isAlive(entity));
  }
}

/**
 * Given three entities with a Position and a Velocity
 * When the first visited entity destroys all the others
 * Then only that entity was visited and only it is alive afterwards
 */
TEST(ComponentQuery, EntityDestroyedDuringVisitIsNotVisitedAfterwards) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  std::vector<Entity> created;
  for (std::size_t count = 0; count < 3; ++count) {
    const Entity entity = entities.create();
    components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});
    components.add<Velocity>(entity, {.x = VELOCITY_X, .y = VELOCITY_Y});
    created.push_back(entity);
  }
  std::vector<Entity> visited;

  components.forEach<Position, Velocity>(
      [&](Entity entity, Position&, Velocity&) {
        visited.push_back(entity);
        for (const Entity& other : created) {
          if (other != entity) {
            components.destroy(other);
          }
        }
      });

  ASSERT_EQ(visited.size(), 1U);
  EXPECT_EQ(entities.size(), 1U);
  EXPECT_TRUE(entities.isAlive(visited.front()));
}

/**
 * Given an entity visited by two nested visits
 * When the inner visit destroys it
 * Then it is still alive when the inner visit returns and destroyed when the
 * outer visit returns
 */
TEST(ComponentQuery, DestructionTakesEffectWhenTheOutermostVisitEnds) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = entities.create();
  components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});
  components.add<Velocity>(entity, {.x = VELOCITY_X, .y = VELOCITY_Y});
  bool aliveAfterInnerVisit = false;

  components.forEach<Position, Velocity>(
      [&](Entity outerEntity, Position&, Velocity&) {
        components.forEach<Position, Velocity>(
            [&](Entity innerEntity, Position&, Velocity&) {
              components.destroy(innerEntity);
            });
        aliveAfterInnerVisit = entities.isAlive(outerEntity);
      });

  EXPECT_TRUE(aliveAfterInnerVisit);
  EXPECT_FALSE(entities.isAlive(entity));
}

/**
 * Given an entity with a Position and a Velocity
 * When it is destroyed twice during the visit
 * Then it is destroyed once and a new entity reusing its index has no
 * component
 */
TEST(ComponentQuery, DestroyingTwiceDuringOneVisitIsHarmless) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = entities.create();
  components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});
  components.add<Velocity>(entity, {.x = VELOCITY_X, .y = VELOCITY_Y});

  components.forEach<Position, Velocity>(
      [&components](Entity visited, Position&, Velocity&) {
        components.destroy(visited);
        components.destroy(visited);
      });
  const Entity recycled = entities.create();

  EXPECT_FALSE(entities.isAlive(entity));
  ASSERT_EQ(recycled.index, entity.index);
  EXPECT_FALSE(components.has<Position>(recycled));
  EXPECT_EQ(entities.size(), 1U);
}

/**
 * Given a destroyed entity whose index was recycled by an entity with a Health
 * When the visit destroys the old entity through its stale handle
 * Then the entity that recycled the index stays alive with its Health
 */
TEST(ComponentQuery, StaleHandleDestroyedDuringVisitSparesRecycledEntity) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity stale = entities.create();
  components.destroy(stale);
  const Entity recycled = entities.create();
  components.add<Health>(recycled, {.points = LOW_HEALTH});
  components.add<Position>(recycled, {.x = FIRST_X, .y = FIRST_Y});

  components.forEach<Position, Health>(
      [&components, stale](Entity, Position&, Health&) {
        components.destroy(stale);
      });

  EXPECT_TRUE(entities.isAlive(recycled));
  ASSERT_NE(components.get<Health>(recycled), nullptr);
  EXPECT_EQ(components.get<Health>(recycled)->points, LOW_HEALTH);
}

/**
 * Given an entity that the callback destroys before throwing
 * When the visit is run
 * Then the exception propagates, the entity is destroyed and components can be
 * added again
 */
TEST(ComponentQuery, DestructionIsAppliedEvenWhenCallbackThrows) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = entities.create();
  components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});
  components.add<Velocity>(entity, {.x = VELOCITY_X, .y = VELOCITY_Y});

  EXPECT_THROW((components.forEach<Position, Velocity>(
                   [&components](Entity visited, Position&, Velocity&) {
                     components.destroy(visited);
                     throw CallbackFailure{};
                   })),
               CallbackFailure);

  EXPECT_FALSE(entities.isAlive(entity));
  const Entity other = entities.create();
  EXPECT_NO_THROW(components.add<Health>(other, {.points = FULL_HEALTH}));
}

/**
 * Given an entity with a Position and a Velocity
 * When a component is added during the visit
 * Then the addition is refused
 */
TEST(ComponentQuery, AddingComponentDuringVisitThrows) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = entities.create();
  components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});
  components.add<Velocity>(entity, {.x = VELOCITY_X, .y = VELOCITY_Y});

  EXPECT_THROW((components.forEach<Position, Velocity>(
                   [&components](Entity visited, Position&, Velocity&) {
                     components.add<Health>(visited, {.points = FULL_HEALTH});
                   })),
               ComponentChangeDuringIterationException);
}

/**
 * Given an entity with a Position and a Velocity
 * When a component is removed during the visit
 * Then the removal is refused and the component is still there
 */
TEST(ComponentQuery, RemovingComponentDuringVisitThrows) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = entities.create();
  components.add<Position>(entity, {.x = FIRST_X, .y = FIRST_Y});
  components.add<Velocity>(entity, {.x = VELOCITY_X, .y = VELOCITY_Y});

  EXPECT_THROW((components.forEach<Position, Velocity>(
                   [&components](Entity visited, Position&, Velocity&) {
                     components.remove<Position>(visited);
                   })),
               ComponentChangeDuringIterationException);
  EXPECT_TRUE(components.has<Position>(entity));
}
