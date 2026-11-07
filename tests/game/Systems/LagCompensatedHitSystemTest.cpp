#include <gtest/gtest.h>
#include "Entity.hpp"
#include "LagCompensatedHitSystemFixture.hpp"
#include "LagCompensatedHitSystemTestConstants.hpp"

using rtype::engine::Entity;
using rtype::game::testing::FIVE_TICKS_BEHIND;
using rtype::game::testing::LagCompensatedHitSystemFixture;
using rtype::game::testing::THIRTEEN_TICKS_BEHIND;
using rtype::game::testing::TICKS_GONE_AFTER;
using rtype::game::testing::TICKS_GONE_BEFORE;
using rtype::game::testing::TWELVE_TICKS_BEHIND;
using rtype::game::testing::TWENTY_TICKS_BEHIND;
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
 * eleven ticks
 * When a missile reaches that place, its player twelve ticks behind
 * Then it hits the enemy: twelve ticks, 200 ms, is the most the server looks
 * back, and it does
 */
TEST_F(LagCompensatedHitSystemFixture, LooksBackTwelveTicks) {
  const Entity enemy = enemyAt(WHERE_THE_PLAYER_SAW_THE_ENEMY);
  runTick();
  move(enemy, WHERE_THE_ENEMY_WENT);
  runTicks(TWELVE_TICKS_BEHIND - 1);
  static_cast<void>(
      missileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, TWELVE_TICKS_BEHIND));

  runTick();

  EXPECT_EQ(hits().size(), 1U);
}

/**
 * Given an enemy that stood at a place for one tick, then went higher for
 * twelve ticks
 * When a missile reaches that place, its player thirteen ticks behind
 * Then it hits nothing: the server looks back twelve ticks, where the enemy
 * had already left
 */
TEST_F(LagCompensatedHitSystemFixture, LooksBackNoMoreThanTwelveTicks) {
  const Entity enemy = enemyAt(WHERE_THE_PLAYER_SAW_THE_ENEMY);
  runTick();
  move(enemy, WHERE_THE_ENEMY_WENT);
  runTicks(TWELVE_TICKS_BEHIND);
  static_cast<void>(
      missileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, THIRTEEN_TICKS_BEHIND));

  runTick();

  EXPECT_TRUE(hits().empty());
}

/**
 * Given an enemy gone for five ticks, then at a place for one tick, then gone
 * again for six ticks
 * When a missile reaches that place, its player claiming to be twenty ticks
 * behind
 * Then it hits nothing: the server looks back twelve ticks at most, whatever
 * the client claims
 */
TEST_F(LagCompensatedHitSystemFixture, AClaimFarInThePastLooksBackTwelveTicks) {
  const Entity enemy = enemyAt(WHERE_THE_ENEMY_WENT);
  runTicks(TICKS_GONE_BEFORE);
  move(enemy, WHERE_THE_PLAYER_SAW_THE_ENEMY);
  runTick();
  move(enemy, WHERE_THE_ENEMY_WENT);
  runTicks(TICKS_GONE_AFTER);
  static_cast<void>(
      missileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, TWENTY_TICKS_BEHIND));

  runTick();

  EXPECT_TRUE(hits().empty());
}
