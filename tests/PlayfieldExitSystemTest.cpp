#include <gtest/gtest.h>
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityType.hpp"
#include "PlayfieldExitSystemFixture.hpp"
#include "PlayfieldExitSystemTestConstants.hpp"

using rtype::engine::Entity;
using rtype::game::DestructionCause;
using rtype::game::EntityDestruction;
using rtype::game::EntityType;
using rtype::game::testing::ENEMY_MISSILE_EXACTLY_AT_THE_MARGIN_X;
using rtype::game::testing::ENEMY_MISSILE_JUST_PAST_THE_MARGIN_X;
using rtype::game::testing::EXACTLY_AT_THE_MARGIN_X;
using rtype::game::testing::EXITING_NETWORK_ID;
using rtype::game::testing::FAR_PAST_THE_EDGE_X;
using rtype::game::testing::JUST_PAST_THE_MARGIN_X;
using rtype::game::testing::MISSILE_EXACTLY_AT_THE_MARGIN_X;
using rtype::game::testing::MISSILE_JUST_PAST_THE_MARGIN_X;
using rtype::game::testing::PlayfieldExitSystemFixture;
using rtype::game::testing::WHERE_ENEMIES_APPEAR_X;

/**
 * Given an enemy 65 units past the left edge
 * When the system runs
 * Then the enemy is gone and its removal was announced under its network id
 */
TEST_F(PlayfieldExitSystemFixture, RemovesAnEnemyPastTheLeftEdge) {
  const Entity enemy = add(EntityType::Enemy, JUST_PAST_THE_MARGIN_X);

  updateAndDispatch();

  EXPECT_FALSE(entities().isAlive(enemy));
  ASSERT_EQ(received().size(), 1U);
  EXPECT_EQ(received().front(),
            (EntityDestruction{.networkId = EXITING_NETWORK_ID,
                               .cause = DestructionCause::Removed}));
}

/**
 * Given an enemy exactly 64 units past the left edge
 * When the system runs
 * Then it is still in the world and nothing was announced
 */
TEST_F(PlayfieldExitSystemFixture, KeepsAnEnemyExactlyAtTheMargin) {
  const Entity enemy = add(EntityType::Enemy, EXACTLY_AT_THE_MARGIN_X);

  updateAndDispatch();

  EXPECT_TRUE(entities().isAlive(enemy));
  EXPECT_TRUE(received().empty());
}

/**
 * Given a ship far past the left edge
 * When the system runs
 * Then the ship is still in the world
 */
TEST_F(PlayfieldExitSystemFixture, NeverRemovesAShip) {
  const Entity ship = add(EntityType::Player, FAR_PAST_THE_EDGE_X);

  updateAndDispatch();

  EXPECT_TRUE(entities().isAlive(ship));
  EXPECT_TRUE(received().empty());
}

/**
 * Given an enemy that was removed by a first run
 * When the system runs again
 * Then the removal was announced once, not twice
 */
TEST_F(PlayfieldExitSystemFixture, AnnouncesARemovalOnlyOnce) {
  static_cast<void>(add(EntityType::Enemy, JUST_PAST_THE_MARGIN_X));
  updateAndDispatch();

  updateAndDispatch();

  EXPECT_EQ(received().size(), 1U);
}

/**
 * Given a missile of a player one unit past the margin on the right
 * When the system runs
 * Then the missile is gone and its removal was announced under its network id
 */
TEST_F(PlayfieldExitSystemFixture, RemovesAPlayerMissilePastTheRightEdge) {
  const Entity missile =
      add(EntityType::PlayerMissile, MISSILE_JUST_PAST_THE_MARGIN_X);

  updateAndDispatch();

  EXPECT_FALSE(entities().isAlive(missile));
  ASSERT_EQ(received().size(), 1U);
  EXPECT_EQ(received().front(),
            (EntityDestruction{.networkId = EXITING_NETWORK_ID,
                               .cause = DestructionCause::Removed}));
}

/**
 * Given a missile of a player exactly at the margin on the right
 * When the system runs
 * Then it is still in the world and nothing was announced
 */
TEST_F(PlayfieldExitSystemFixture, KeepsAPlayerMissileExactlyAtTheMargin) {
  const Entity missile =
      add(EntityType::PlayerMissile, MISSILE_EXACTLY_AT_THE_MARGIN_X);

  updateAndDispatch();

  EXPECT_TRUE(entities().isAlive(missile));
  EXPECT_TRUE(received().empty());
}

/**
 * Given an enemy past the right edge, where the Bydos appear
 * When the system runs
 * Then it is still in the world: only a missile of a player leaves on the right
 */
TEST_F(PlayfieldExitSystemFixture, KeepsAnEnemyThatHasNotComeInYet) {
  const Entity enemy = add(EntityType::Enemy, WHERE_ENEMIES_APPEAR_X);

  updateAndDispatch();

  EXPECT_TRUE(entities().isAlive(enemy));
  EXPECT_TRUE(received().empty());
}

/**
 * Given a missile of an enemy one unit past the margin on the left
 * When the system runs
 * Then the missile is gone and its removal was announced under its network id
 */
TEST_F(PlayfieldExitSystemFixture, RemovesAnEnemyMissilePastTheLeftEdge) {
  const Entity missile =
      add(EntityType::EnemyMissile, ENEMY_MISSILE_JUST_PAST_THE_MARGIN_X);

  updateAndDispatch();

  EXPECT_FALSE(entities().isAlive(missile));
  ASSERT_EQ(received().size(), 1U);
  EXPECT_EQ(received().front(),
            (EntityDestruction{.networkId = EXITING_NETWORK_ID,
                               .cause = DestructionCause::Removed}));
}

/**
 * Given a missile of an enemy exactly at the margin on the left
 * When the system runs
 * Then it is still in the world and nothing was announced
 */
TEST_F(PlayfieldExitSystemFixture, KeepsAnEnemyMissileExactlyAtTheMargin) {
  const Entity missile =
      add(EntityType::EnemyMissile, ENEMY_MISSILE_EXACTLY_AT_THE_MARGIN_X);

  updateAndDispatch();

  EXPECT_TRUE(entities().isAlive(missile));
  EXPECT_TRUE(received().empty());
}

/**
 * Given a missile of an enemy past the right edge, where the Bydos appear
 * When the system runs
 * Then it is still in the world: it flies left, into the playfield
 */
TEST_F(PlayfieldExitSystemFixture, KeepsAnEnemyMissileThatHasNotComeInYet) {
  const Entity missile = add(EntityType::EnemyMissile, WHERE_ENEMIES_APPEAR_X);

  updateAndDispatch();

  EXPECT_TRUE(entities().isAlive(missile));
  EXPECT_TRUE(received().empty());
}
