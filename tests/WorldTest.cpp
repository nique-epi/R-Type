#include <gtest/gtest.h>
#include "World.hpp"

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
