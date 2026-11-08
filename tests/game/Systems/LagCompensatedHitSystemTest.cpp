#include <gtest/gtest.h>
#include "Entity.hpp"
#include "LagCompensatedHitSystemFixture.hpp"
#include "LagCompensatedHitSystemTestConstants.hpp"

using rtype::engine::Entity;
using rtype::game::testing::FAR_CLAIM_TICKS_BEHIND;
using rtype::game::testing::FIVE_TICKS_BEHIND;
using rtype::game::testing::KEPT_TICKS_BEHIND;
using rtype::game::testing::LagCompensatedHitSystemFixture;
using rtype::game::testing::PAST_THE_KEPT_TICKS_BEHIND;
using rtype::game::testing::TICKS_GONE_AFTER;
using rtype::game::testing::TICKS_GONE_BEFORE;
using rtype::game::testing::WHERE_THE_ENEMY_WENT;
using rtype::game::testing::WHERE_THE_PLAYER_SAW_THE_ENEMY;

/**
 * Given an enemy and a missile on it, whose player sees the present
 * When one tick runs
 * Then the missile hits that enemy
 */
TEST_F(LagCompensatedHitSystemFixture, AMissileWithoutDelayHitsWhatItTouches) {
  const Entity enemy = enemyAt(WHERE_THE_PLAYER_SAW_THE_ENEMY);
  const Entity missile = missileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, 0);

  runTick();

  ASSERT_EQ(hits().size(), 1U);
  EXPECT_EQ(hits().front().missile, missile);
  EXPECT_EQ(hits().front().target, enemy);
}

/**
 * Given an enemy that stood at a place for one tick, then went 200 units
 * higher for four ticks
 * When a missile reaches that place, its player five ticks behind
 * Then it hits the enemy: its player saw the enemy there
 */
TEST_F(LagCompensatedHitSystemFixture, AMissileHitsWhereItsPlayerSawTheEnemy) {
  const Entity enemy = enemyAt(WHERE_THE_PLAYER_SAW_THE_ENEMY);
  runTick();
  move(enemy, WHERE_THE_ENEMY_WENT);
  runTicks(FIVE_TICKS_BEHIND - 1);
  static_cast<void>(
      missileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, FIVE_TICKS_BEHIND));

  runTick();

  ASSERT_EQ(hits().size(), 1U);
  EXPECT_EQ(hits().front().target, enemy);
}

/**
 * Given the same enemy, gone 200 units higher for four ticks
 * When a missile reaches the place it left, its player seeing the present
 * Then it hits nothing: the enemy is no longer there in the present
 */
TEST_F(LagCompensatedHitSystemFixture, AMissileWithoutDelayMissesWhatLeft) {
  const Entity enemy = enemyAt(WHERE_THE_PLAYER_SAW_THE_ENEMY);
  runTick();
  move(enemy, WHERE_THE_ENEMY_WENT);
  runTicks(FIVE_TICKS_BEHIND - 1);
  static_cast<void>(missileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, 0));

  runTick();

  EXPECT_TRUE(hits().empty());
}

/**
 * Given an enemy that stood at a place for one tick, then was destroyed
 * When a missile reaches that place, its player one tick behind
 * Then it hits nothing: a missile never kills what is already dead
 */
TEST_F(LagCompensatedHitSystemFixture, AMissileNeverHitsAnEnemyDestroyedSince) {
  const Entity enemy = enemyAt(WHERE_THE_PLAYER_SAW_THE_ENEMY);
  runTick();
  components().destroy(enemy);
  static_cast<void>(missileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, 1));

  runTick();

  EXPECT_TRUE(hits().empty());
}

/**
 * Given an enemy that stood at a place for one tick, then went higher for
 * as many ticks as the system keeps, less one
 * When a missile reaches that place, its player that many ticks behind
 * Then it hits the enemy: the system looks as far back as it keeps
 */
TEST_F(LagCompensatedHitSystemFixture, LooksBackAsFarAsItKeeps) {
  const Entity enemy = enemyAt(WHERE_THE_PLAYER_SAW_THE_ENEMY);
  runTick();
  move(enemy, WHERE_THE_ENEMY_WENT);
  runTicks(KEPT_TICKS_BEHIND - 1);
  static_cast<void>(
      missileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, KEPT_TICKS_BEHIND));

  runTick();

  EXPECT_EQ(hits().size(), 1U);
}

/**
 * Given an enemy that stood at a place for one tick, then went higher for as
 * many ticks as the system keeps
 * When a missile reaches that place, its player one tick further behind
 * Then it hits nothing: the system looks back as far as it keeps, where the
 * enemy had already left
 */
TEST_F(LagCompensatedHitSystemFixture, LooksBackNoFurtherThanItKeeps) {
  const Entity enemy = enemyAt(WHERE_THE_PLAYER_SAW_THE_ENEMY);
  runTick();
  move(enemy, WHERE_THE_ENEMY_WENT);
  runTicks(KEPT_TICKS_BEHIND);
  static_cast<void>(
      missileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, PAST_THE_KEPT_TICKS_BEHIND));

  runTick();

  EXPECT_TRUE(hits().empty());
}

/**
 * Given an enemy gone for as many ticks as the system keeps, then at a place
 * for one tick, then gone again for six ticks
 * When a missile reaches that place, its player claiming to be eight ticks
 * further behind than the system keeps
 * Then it hits nothing: the system looks back as far as it keeps at most,
 * whatever the client claims
 */
TEST_F(LagCompensatedHitSystemFixture,
       AClaimFarInThePastLooksBackAsFarAsItKeeps) {
  const Entity enemy = enemyAt(WHERE_THE_ENEMY_WENT);
  runTicks(TICKS_GONE_BEFORE);
  move(enemy, WHERE_THE_PLAYER_SAW_THE_ENEMY);
  runTick();
  move(enemy, WHERE_THE_ENEMY_WENT);
  runTicks(TICKS_GONE_AFTER);
  static_cast<void>(
      missileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, FAR_CLAIM_TICKS_BEHIND));

  runTick();

  EXPECT_TRUE(hits().empty());
}

/**
 * Given an enemy recorded in the world of a game that ended
 * When a new world starts, with an enemy 200 units higher, and a missile one
 * tick behind reaches where the old enemy stood
 * Then it hits nothing: the enemies of the old world are gone with it
 */
TEST_F(LagCompensatedHitSystemFixture, ANewWorldForgetsTheEnemiesOfTheOldOne) {
  enemyAt(WHERE_THE_PLAYER_SAW_THE_ENEMY);
  runTick();
  startANewWorld();
  enemyAt(WHERE_THE_ENEMY_WENT);
  missileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, 1);

  runTick();

  EXPECT_TRUE(hits().empty());
}
