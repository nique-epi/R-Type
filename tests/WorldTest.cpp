#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include "BydoKind.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityIdConstants.hpp"
#include "EntityType.hpp"
#include "MovementPattern.hpp"
#include "NetworkIdentity.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "World.hpp"

namespace {

constexpr float SOME_X = 3.0F;
constexpr float SOME_Y = 4.0F;

}  // namespace

/**
 * Given an empty world
 * When a player is spawned at a position
 * Then the world holds one entity
 */
TEST(World, SpawnedPlayerIsCounted) {
  rtype::game::World world;

  world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_EQ(world.entityCount(), 1U);
}

/**
 * Given an empty world
 * When two players are spawned
 * Then they get two different network ids, none of them NO_ENTITY_ID
 */
TEST(World, SpawnedPlayersGetDistinctNetworkIds) {
  rtype::game::World world;

  const std::uint32_t first =
      world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});
  const std::uint32_t second =
      world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_NE(first, rtype::identifiers::NO_ENTITY_ID);
  EXPECT_NE(second, rtype::identifiers::NO_ENTITY_ID);
  EXPECT_NE(first, second);
}

/**
 * Given a world
 * When a player is spawned at a position
 * Then the ship has that position, the player type and the network id returned
 */
TEST(World, SpawnedPlayerIsANetworkedShipAtItsPosition) {
  rtype::game::World world;

  const std::uint32_t networkId =
      world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  bool found = false;
  world.components()
      .forEach<rtype::game::NetworkIdentity, rtype::game::Position>(
          [&world, &found, networkId](
              rtype::engine::Entity entity,
              const rtype::game::NetworkIdentity& identity,
              const rtype::game::Position& position) {
            found = true;
            EXPECT_EQ(identity.networkId, networkId);
            EXPECT_EQ(position.x, SOME_X);
            EXPECT_EQ(position.y, SOME_Y);
            const auto* type =
                world.components().get<rtype::game::EntityType>(entity);
            ASSERT_NE(type, nullptr);
            EXPECT_EQ(*type, rtype::game::EntityType::Player);
          });
  EXPECT_TRUE(found);
}

/**
 * Given an empty world
 * When a Bydo is spawned at a position
 * Then it is an enemy at that position, under the network id returned
 */
TEST(World, SpawnedBydoIsAnEnemyAtItsPosition) {
  rtype::game::World world;

  const std::uint32_t networkId =
      world.spawnBydo(rtype::game::Position{.x = SOME_X, .y = SOME_Y},
                      rtype::game::BydoKind::Drifter);

  EXPECT_EQ(world.entityCount(), 1U);
  EXPECT_NE(networkId, rtype::identifiers::NO_ENTITY_ID);
  bool found = false;
  world.components()
      .forEach<rtype::game::NetworkIdentity, rtype::game::Position>(
          [&world, &found, networkId](
              rtype::engine::Entity entity,
              const rtype::game::NetworkIdentity& identity,
              const rtype::game::Position& position) {
            found = true;
            EXPECT_EQ(identity.networkId, networkId);
            EXPECT_EQ(position.x, SOME_X);
            EXPECT_EQ(position.y, SOME_Y);
            const auto* type =
                world.components().get<rtype::game::EntityType>(entity);
            ASSERT_NE(type, nullptr);
            EXPECT_EQ(*type, rtype::game::EntityType::Enemy);
          });
  EXPECT_TRUE(found);
}

/**
 * Given a world with a player and a Bydo
 * When their network ids are compared
 * Then they differ, so the clients can tell the two entities apart
 */
TEST(World, ABydoAndAPlayerGetDistinctNetworkIds) {
  rtype::game::World world;

  const std::uint32_t player =
      world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});
  const std::uint32_t bydo =
      world.spawnBydo(rtype::game::Position{.x = SOME_X, .y = SOME_Y},
                      rtype::game::BydoKind::Drifter);

  EXPECT_NE(player, bydo);
}

/**
 * Given an empty world
 * When a Drifter and a Weaver are spawned on the row at height SOME_Y
 * Then both move left at 240 units per second, the Drifter goes straight, the
 * Weaver follows a wave, and both start from that row
 */
TEST(World, SpawnedBydosMoveLeftAlongThePatternOfTheirKind) {
  rtype::game::World world;
  static_cast<void>(
      world.spawnBydo(rtype::game::Position{.x = SOME_X, .y = SOME_Y},
                      rtype::game::BydoKind::Drifter));
  static_cast<void>(
      world.spawnBydo(rtype::game::Position{.x = SOME_X, .y = SOME_Y},
                      rtype::game::BydoKind::Weaver));

  std::size_t straight = 0;
  std::size_t waves = 0;
  world.components()
      .forEach<rtype::game::MovementPattern, rtype::game::Velocity>(
          [&straight, &waves](rtype::engine::Entity,
                              const rtype::game::MovementPattern& pattern,
                              const rtype::game::Velocity& velocity) {
            EXPECT_EQ(velocity.x, -240.0F);
            EXPECT_EQ(velocity.y, 0.0F);
            EXPECT_EQ(pattern.originY, SOME_Y);
            EXPECT_EQ(pattern.ageSeconds, 0.0F);
            straight += pattern.shape == rtype::game::PatternShape::Straight;
            waves += pattern.shape == rtype::game::PatternShape::Wave;
          });
  EXPECT_EQ(straight, 1U);
  EXPECT_EQ(waves, 1U);
}
