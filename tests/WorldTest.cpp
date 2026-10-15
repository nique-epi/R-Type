#include <gtest/gtest.h>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include "World.hpp"

namespace {

constexpr float SOME_X = 3.0F;
constexpr float SOME_Y = 4.0F;

}  // namespace

/**
 * Given an empty world
 * When a player is spawned
 * Then the world holds one entity
 */
TEST(World, SpawnedPlayerIsCounted) {
  rtype::game::World world;

  world.spawnPlayer();

  EXPECT_EQ(world.entityCount(), 1U);
}

/**
 * Given a world with a spawned player
 * When a component is added to it through components() and read back
 * Then the component is there, so the world's components belong to its entities
 */
TEST(World, OwnsTheComponentsOfItsEntities) {
  rtype::game::World world;
  const rtype::engine::Entity player = world.spawnPlayer();

  world.components().add(player,
                         rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  const rtype::game::Position* position =
      world.components().get<rtype::game::Position>(player);
  ASSERT_NE(position, nullptr);
  EXPECT_EQ(position->x, SOME_X);
}
