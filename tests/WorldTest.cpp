#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "BydoKind.hpp"
#include "BydoPointsConstants.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityIdConstants.hpp"
#include "EntitySizeConstants.hpp"
#include "EntityType.hpp"
#include "Gun.hpp"
#include "Health.hpp"
#include "Intent.hpp"
#include "MissileOwner.hpp"
#include "MovementPattern.hpp"
#include "NetworkIdentity.hpp"
#include "Position.hpp"
#include "ScoreValue.hpp"
#include "Velocity.hpp"
#include "World.hpp"
#include "WorldTestConstants.hpp"

using rtype::game::testing::ENEMY_MISSILE_UNITS_PER_SECOND;
using rtype::game::testing::MISSILE_UNITS_PER_SECOND;
using rtype::game::testing::OTHER_X;
using rtype::game::testing::OTHER_Y;
using rtype::game::testing::SECOND_PLAYER_NUMBER;
using rtype::game::testing::SOME_X;
using rtype::game::testing::SOME_Y;
using rtype::game::testing::THREE_HEALTH_POINTS;

namespace {

/**
 * @returns The Component of the entity of @p world that carries @p networkId,
 * or nothing when no such entity has one.
 */
template <typename Component>
std::optional<Component> componentOf(rtype::game::World& world,
                                     std::uint32_t networkId) {
  std::optional<Component> found;
  world.components().forEach<rtype::game::NetworkIdentity, Component>(
      [&found, networkId](rtype::engine::Entity /*entity*/,
                          const rtype::game::NetworkIdentity& identity,
                          const Component& component) {
        if (identity.networkId == networkId) {
          found = component;
        }
      });
  return found;
}

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
 * Given a world holding the ship of a player
 * When that ship is removed by its network id
 * Then the removal succeeds and the world holds no entity any more
 */
TEST(World, RemovedShipLeavesTheWorld) {
  rtype::game::World world;
  const std::uint32_t ship =
      world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  const bool removed = world.removePlayer(ship);

  EXPECT_TRUE(removed);
  EXPECT_EQ(world.entityCount(), 0U);
}

/**
 * Given a world holding the ships of two players
 * When the first ship is removed
 * Then the second ship is still in the world, with its network id
 */
TEST(World, RemovesOnlyTheShipItIsGiven) {
  rtype::game::World world;
  const std::uint32_t first =
      world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});
  const std::uint32_t second =
      world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  world.removePlayer(first);

  std::vector<std::uint32_t> networkIds;
  world.components()
      .forEach<rtype::game::NetworkIdentity, rtype::game::EntityType>(
          [&networkIds](rtype::engine::Entity /*entity*/,
                        const rtype::game::NetworkIdentity& identity,
                        const rtype::game::EntityType& /*type*/) {
            networkIds.push_back(identity.networkId);
          });
  EXPECT_EQ(networkIds, std::vector<std::uint32_t>{second});
}

/**
 * Given a world whose only ship was removed
 * When the same network id is removed again
 * Then nothing is removed: the id no longer names a ship
 */
TEST(World, RemovesAShipOnlyOnce) {
  rtype::game::World world;
  const std::uint32_t ship =
      world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});
  ASSERT_TRUE(world.removePlayer(ship));

  const bool removedAgain = world.removePlayer(ship);

  EXPECT_FALSE(removedAgain);
}

/**
 * Given a world holding the ship of a player
 * When the id no entity has is removed
 * Then nothing is removed and the ship stays
 */
TEST(World, RemovesNothingForTheIdNoEntityHas) {
  rtype::game::World world;
  world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  const bool removed = world.removePlayer(rtype::identifiers::NO_ENTITY_ID);

  EXPECT_FALSE(removed);
  EXPECT_EQ(world.entityCount(), 1U);
}

/**
 * Given a world holding a Bydo
 * When the network id of that Bydo is removed as a ship
 * Then nothing is removed: only the ship of a player leaves this way
 */
TEST(World, RemovesNoBydoAsAShip) {
  rtype::game::World world;
  const std::uint32_t bydo =
      world.spawnBydo(rtype::game::Position{.x = SOME_X, .y = SOME_Y},
                      rtype::game::BydoKind::Drifter);

  const bool removed = world.removePlayer(bydo);

  EXPECT_FALSE(removed);
  EXPECT_EQ(world.entityCount(), 1U);
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

/**
 * Given an empty world
 * When a Bydo is spawned
 * Then it has three health points
 */
TEST(World, SpawnedBydoHasThreeHealthPoints) {
  rtype::game::World world;

  static_cast<void>(
      world.spawnBydo(rtype::game::Position{.x = SOME_X, .y = SOME_Y},
                      rtype::game::BydoKind::Drifter));

  int bydosWithHealth = 0;
  world.components().forEach<rtype::game::EntityType, rtype::game::Health>(
      [&bydosWithHealth](rtype::engine::Entity /*entity*/,
                         const rtype::game::EntityType& type,
                         const rtype::game::Health& health) {
        ++bydosWithHealth;
        EXPECT_EQ(type, rtype::game::EntityType::Enemy);
        EXPECT_EQ(health.points, THREE_HEALTH_POINTS);
      });
  EXPECT_EQ(bydosWithHealth, 1);
}

/**
 * Given an empty world
 * When the ship of a player is spawned
 * Then it has no health: a hit costs a player a life, not health points
 */
TEST(World, SpawnedShipHasNoHealth) {
  rtype::game::World world;

  world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  int shipsWithHealth = 0;
  world.components().forEach<rtype::game::EntityType, rtype::game::Health>(
      [&shipsWithHealth](rtype::engine::Entity /*entity*/,
                         const rtype::game::EntityType& /*type*/,
                         const rtype::game::Health& /*health*/) {
        ++shipsWithHealth;
      });
  EXPECT_EQ(shipsWithHealth, 0);
}

/**
 * Given an empty world
 * When a Drifter and a Weaver are spawned
 * Then each is worth the points of its kind
 */
TEST(World, SpawnedBydoIsWorthThePointsOfItsKind) {
  rtype::game::World world;
  const rtype::game::Position position{.x = SOME_X, .y = SOME_Y};

  const std::uint32_t drifterId =
      world.spawnBydo(position, rtype::game::BydoKind::Drifter);
  const std::uint32_t weaverId =
      world.spawnBydo(position, rtype::game::BydoKind::Weaver);

  std::uint32_t drifterPoints = 0;
  std::uint32_t weaverPoints = 0;
  world.components()
      .forEach<rtype::game::NetworkIdentity, rtype::game::ScoreValue>(
          [&](rtype::engine::Entity,
              const rtype::game::NetworkIdentity& identity,
              const rtype::game::ScoreValue& score) {
            (identity.networkId == drifterId ? drifterPoints : weaverPoints) =
                score.points;
          });
  EXPECT_EQ(drifterPoints, rtype::game::DRIFTER_POINTS);
  EXPECT_EQ(weaverPoints, rtype::game::WEAVER_POINTS);
  EXPECT_NE(drifterId, weaverId);
}

/**
 * Given an empty world
 * When a Bydo is spawned
 * Then it collides with a box of the size of its sprite
 */
TEST(World, SpawnedBydoHasTheBoxOfItsSprite) {
  rtype::game::World world;

  const std::uint32_t networkId =
      world.spawnBydo(rtype::game::Position{.x = SOME_X, .y = SOME_Y},
                      rtype::game::BydoKind::Drifter);

  const std::optional<rtype::game::CollisionBox> box =
      componentOf<rtype::game::CollisionBox>(world, networkId);
  ASSERT_TRUE(box.has_value());
  const rtype::game::CollisionBox found =
      box.value_or(rtype::game::CollisionBox{});
  EXPECT_EQ(found.width, rtype::game::BYDO_WIDTH);
  EXPECT_EQ(found.height, rtype::game::BYDO_HEIGHT);
}

/**
 * Given a world where a ship was spawned at a position
 * When the position of its network id is asked
 * Then it is the position of that ship
 */
TEST(World, GivesThePositionOfAShipByItsNetworkId) {
  rtype::game::World world;
  const std::uint32_t ship =
      world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  const rtype::game::Position* const position = world.shipPositionOf(ship);

  ASSERT_NE(position, nullptr);
  EXPECT_EQ(position->x, SOME_X);
  EXPECT_EQ(position->y, SOME_Y);
}

/**
 * Given a world where two ships were spawned at two positions
 * When the position of the network id of the second is asked
 * Then it is the position of the second ship, not of the first
 */
TEST(World, GivesEachShipItsOwnPosition) {
  rtype::game::World world;
  world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});
  const std::uint32_t second =
      world.spawnPlayer(rtype::game::Position{.x = OTHER_X, .y = OTHER_Y});

  const rtype::game::Position* const position = world.shipPositionOf(second);

  ASSERT_NE(position, nullptr);
  EXPECT_EQ(position->x, OTHER_X);
  EXPECT_EQ(position->y, OTHER_Y);
}

/**
 * Given a world where a Bydo was spawned
 * When the ship position of its network id is asked
 * Then there is none: a Bydo is not a ship
 */
TEST(World, GivesNoShipPositionForTheIdOfABydo) {
  rtype::game::World world;
  const std::uint32_t bydo =
      world.spawnBydo(rtype::game::Position{.x = SOME_X, .y = SOME_Y},
                      rtype::game::BydoKind::Drifter);

  EXPECT_EQ(world.shipPositionOf(bydo), nullptr);
}

/**
 * Given a world where one ship was spawned
 * When the position of the next network id is asked
 * Then there is none: no entity was given that id
 */
TEST(World, GivesNoShipPositionForAnIdItNeverGave) {
  rtype::game::World world;
  const std::uint32_t ship =
      world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_EQ(world.shipPositionOf(ship + 1), nullptr);
}

/**
 * Given a world where one ship was spawned
 * When the position of NO_ENTITY_ID is asked
 * Then there is none
 */
TEST(World, GivesNoShipPositionForNoEntityId) {
  rtype::game::World world;
  world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_EQ(world.shipPositionOf(rtype::identifiers::NO_ENTITY_ID), nullptr);
}

/**
 * Given an empty world
 * When the second player fires a missile at a position
 * Then the world holds a player missile at that position, under the network id
 * returned
 */
TEST(World, SpawnedPlayerMissileIsANetworkedMissileAtItsPosition) {
  rtype::game::World world;

  const std::uint32_t networkId = world.spawnPlayerMissile(
      rtype::game::Position{.x = SOME_X, .y = SOME_Y}, SECOND_PLAYER_NUMBER);

  EXPECT_EQ(world.entityCount(), 1U);
  EXPECT_NE(networkId, rtype::identifiers::NO_ENTITY_ID);
  const std::optional<rtype::game::Position> position =
      componentOf<rtype::game::Position>(world, networkId);
  ASSERT_TRUE(position.has_value());
  const rtype::game::Position found =
      position.value_or(rtype::game::Position{});
  EXPECT_EQ(found.x, SOME_X);
  EXPECT_EQ(found.y, SOME_Y);
  EXPECT_EQ(componentOf<rtype::game::EntityType>(world, networkId),
            rtype::game::EntityType::PlayerMissile);
}

/**
 * Given an empty world
 * When the second player fires a missile
 * Then the missile belongs to the second player
 */
TEST(World, SpawnedPlayerMissileBelongsToThePlayerWhoFired) {
  rtype::game::World world;

  const std::uint32_t networkId = world.spawnPlayerMissile(
      rtype::game::Position{.x = SOME_X, .y = SOME_Y}, SECOND_PLAYER_NUMBER);

  const std::optional<rtype::game::MissileOwner> owner =
      componentOf<rtype::game::MissileOwner>(world, networkId);
  ASSERT_TRUE(owner.has_value());
  EXPECT_EQ(owner.value_or(rtype::game::MissileOwner{}).playerNumber,
            SECOND_PLAYER_NUMBER);
}

/**
 * Given an empty world
 * When a player fires a missile
 * Then the missile flies right at 960 units per second, on its row
 */
TEST(World, SpawnedPlayerMissileFliesRight) {
  rtype::game::World world;

  const std::uint32_t networkId = world.spawnPlayerMissile(
      rtype::game::Position{.x = SOME_X, .y = SOME_Y}, SECOND_PLAYER_NUMBER);

  const std::optional<rtype::game::Velocity> velocity =
      componentOf<rtype::game::Velocity>(world, networkId);
  ASSERT_TRUE(velocity.has_value());
  const rtype::game::Velocity found =
      velocity.value_or(rtype::game::Velocity{});
  EXPECT_EQ(found.x, MISSILE_UNITS_PER_SECOND);
  EXPECT_EQ(found.y, 0.0F);
}

/**
 * Given an empty world
 * When a player fires a missile
 * Then the missile collides with a box of the size of its sprite
 */
TEST(World, SpawnedPlayerMissileHasTheBoxOfItsSprite) {
  rtype::game::World world;

  const std::uint32_t networkId = world.spawnPlayerMissile(
      rtype::game::Position{.x = SOME_X, .y = SOME_Y}, SECOND_PLAYER_NUMBER);

  const std::optional<rtype::game::CollisionBox> box =
      componentOf<rtype::game::CollisionBox>(world, networkId);
  ASSERT_TRUE(box.has_value());
  const rtype::game::CollisionBox found =
      box.value_or(rtype::game::CollisionBox{});
  EXPECT_EQ(found.width, rtype::game::PLAYER_MISSILE_WIDTH);
  EXPECT_EQ(found.height, rtype::game::PLAYER_MISSILE_HEIGHT);
}

/**
 * Given an empty world
 * When a Bydo is spawned
 * Then it carries a gun that has not fired yet, so its first missile waits a
 * whole interval
 */
TEST(World, SpawnedBydoHasAGunThatHasNotFiredYet) {
  rtype::game::World world;

  const std::uint32_t networkId =
      world.spawnBydo(rtype::game::Position{.x = SOME_X, .y = SOME_Y},
                      rtype::game::BydoKind::Drifter);

  const std::optional<rtype::game::Gun> gun =
      componentOf<rtype::game::Gun>(world, networkId);
  ASSERT_TRUE(gun.has_value());
  EXPECT_EQ(gun.value_or(rtype::game::Gun{}).ticksSinceLastShot, 0U);
}

/**
 * Given an empty world
 * When an enemy fires a missile at a position
 * Then the world holds a missile of an enemy at that position, under the
 * network id returned
 */
TEST(World, SpawnedEnemyMissileIsANetworkedMissileAtItsPosition) {
  rtype::game::World world;

  const std::uint32_t networkId =
      world.spawnEnemyMissile(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_EQ(world.entityCount(), 1U);
  EXPECT_NE(networkId, rtype::identifiers::NO_ENTITY_ID);
  const std::optional<rtype::game::Position> position =
      componentOf<rtype::game::Position>(world, networkId);
  ASSERT_TRUE(position.has_value());
  const rtype::game::Position found =
      position.value_or(rtype::game::Position{});
  EXPECT_EQ(found.x, SOME_X);
  EXPECT_EQ(found.y, SOME_Y);
  EXPECT_EQ(componentOf<rtype::game::EntityType>(world, networkId),
            rtype::game::EntityType::EnemyMissile);
}

/**
 * Given an empty world
 * When an enemy fires a missile
 * Then the missile flies left at 360 units per second, on its row
 */
TEST(World, SpawnedEnemyMissileFliesLeft) {
  rtype::game::World world;

  const std::uint32_t networkId =
      world.spawnEnemyMissile(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  const std::optional<rtype::game::Velocity> velocity =
      componentOf<rtype::game::Velocity>(world, networkId);
  ASSERT_TRUE(velocity.has_value());
  const rtype::game::Velocity found =
      velocity.value_or(rtype::game::Velocity{});
  EXPECT_EQ(found.x, -ENEMY_MISSILE_UNITS_PER_SECOND);
  EXPECT_EQ(found.y, 0.0F);
}

/**
 * Given an empty world
 * When an enemy fires a missile
 * Then the missile collides with a box of the size of its sprite
 */
TEST(World, SpawnedEnemyMissileHasTheBoxOfItsSprite) {
  rtype::game::World world;

  const std::uint32_t networkId =
      world.spawnEnemyMissile(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  const std::optional<rtype::game::CollisionBox> box =
      componentOf<rtype::game::CollisionBox>(world, networkId);
  ASSERT_TRUE(box.has_value());
  const rtype::game::CollisionBox found =
      box.value_or(rtype::game::CollisionBox{});
  EXPECT_EQ(found.width, rtype::game::ENEMY_MISSILE_WIDTH);
  EXPECT_EQ(found.height, rtype::game::ENEMY_MISSILE_HEIGHT);
}

/**
 * Given a world
 * When a ship is spawned for a player
 * Then shipIntentOf finds an Intent wanting nothing, and no other id has one
 */
TEST(World, ShipIsSpawnedWithAnIntentThatWantsNothing) {
  rtype::game::World world;
  const std::uint32_t shipId =
      world.spawnPlayer(rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  const rtype::game::Intent* const intent = world.shipIntentOf(shipId);

  ASSERT_NE(intent, nullptr);
  EXPECT_FALSE(intent->directions.up);
  EXPECT_FALSE(intent->directions.down);
  EXPECT_FALSE(intent->directions.left);
  EXPECT_FALSE(intent->directions.right);
  EXPECT_EQ(world.shipIntentOf(shipId + 1), nullptr);
}
