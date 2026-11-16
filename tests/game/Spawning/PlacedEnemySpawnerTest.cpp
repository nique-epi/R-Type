#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <array>
#include "LdtkPlacementFixtures.hpp"
#include "LdtkPlacementTestConstants.hpp"
#include "PlacedEnemySpawner.hpp"
#include "PlacedEnemySpawnerTestConstants.hpp"
#include "RecordingEntityOrders.hpp"

using rtype::game::PlacedEnemySpawner;
using rtype::game::testing::CHECKPOINT_SCROLL;
using rtype::game::testing::COMES_IN_AT_100;
using rtype::game::testing::COMES_IN_AT_300;
using rtype::game::testing::COMES_IN_AT_THE_CHECKPOINT;
using rtype::game::testing::COMES_IN_ONE_TICK_AFTER_THE_CHECKPOINT;
using rtype::game::testing::NO_SCROLL;
using rtype::game::testing::ON_THE_FIRST_SCREEN;
using rtype::game::testing::PAST_THE_FIRST_SCREEN;
using rtype::game::testing::SCROLL_ONE_TICK_BEFORE_IT_COMES_IN;
using rtype::game::testing::SCROLL_PAST_BOTH;
using rtype::game::testing::SCROLL_PAST_IT;
using rtype::game::testing::SCROLL_WHEN_IT_COMES_IN;
using rtype::game::testing::SPAWNED_ENEMY_ID;
using rtype::game::testing::WHERE_IT_APPEARS_X;
using rtype::game::testing::WIDE_ENEMY_CENTER;
using rtype::game::testing::WIDE_WIDTH;
using ::testing::IsEmpty;
using ::testing::SizeIs;

/**
 * Given an enemy placed on the first screen of the level, at x 800
 * When the enemies due are spawned before the level scrolled
 * Then it appears at its place
 */
TEST(PlacedEnemySpawner, SpawnsAnEnemyOnTheFirstScreenAtItsPlaceAtTheStart) {
  const std::array placements{placedEnemyOf(
      PLACED_DRIFTER_SHEET, ON_THE_FIRST_SCREEN, PLACED_ENEMY_WIDTH)};
  PlacedEnemySpawner spawner{placements};
  RecordingEntityOrders orders{SPAWNED_ENEMY_ID};

  spawner.spawnDue(NO_SCROLL, orders);

  ASSERT_THAT(orders.spawns(), SizeIs(1));
  EXPECT_EQ(orders.spawns().front().type, PLACED_DRIFTER_SHEET);
  EXPECT_FLOAT_EQ(orders.spawns().front().position.x, ON_THE_FIRST_SCREEN.x);
  EXPECT_FLOAT_EQ(orders.spawns().front().position.y, ON_THE_FIRST_SCREEN.y);
}

/**
 * Given an enemy placed at x 2224, 96 units wide
 * When the enemies due are spawned one tick of scroll before its left edge
 * reaches the right edge of the playfield
 * Then nothing appears
 */
TEST(PlacedEnemySpawner, HoldsAnEnemyUntilItsLeftEdgeReachesTheRightEdge) {
  const std::array placements{placedEnemyOf(
      PLACED_DRIFTER_SHEET, PAST_THE_FIRST_SCREEN, PLACED_ENEMY_WIDTH)};
  PlacedEnemySpawner spawner{placements};
  RecordingEntityOrders orders{SPAWNED_ENEMY_ID};

  spawner.spawnDue(SCROLL_ONE_TICK_BEFORE_IT_COMES_IN, orders);

  EXPECT_THAT(orders.spawns(), IsEmpty());
}

/**
 * Given the same enemy
 * When the enemies due are spawned once the level scrolled 256 units
 * Then it appears with its left edge on the right edge, at x 1968
 */
TEST(PlacedEnemySpawner, SpawnsAnEnemyWhenItsLeftEdgeReachesTheRightEdge) {
  const std::array placements{placedEnemyOf(
      PLACED_DRIFTER_SHEET, PAST_THE_FIRST_SCREEN, PLACED_ENEMY_WIDTH)};
  PlacedEnemySpawner spawner{placements};
  RecordingEntityOrders orders{SPAWNED_ENEMY_ID};

  spawner.spawnDue(SCROLL_WHEN_IT_COMES_IN, orders);

  ASSERT_THAT(orders.spawns(), SizeIs(1));
  EXPECT_FLOAT_EQ(orders.spawns().front().position.x, WHERE_IT_APPEARS_X);
}

/**
 * Given an enemy that came in
 * When the enemies due are spawned again, further on
 * Then it does not appear a second time
 */
TEST(PlacedEnemySpawner, SpawnsEachEnemyOnce) {
  const std::array placements{placedEnemyOf(
      PLACED_DRIFTER_SHEET, PAST_THE_FIRST_SCREEN, PLACED_ENEMY_WIDTH)};
  PlacedEnemySpawner spawner{placements};
  RecordingEntityOrders orders{SPAWNED_ENEMY_ID};
  spawner.spawnDue(SCROLL_PAST_IT, orders);

  spawner.spawnDue(SCROLL_PAST_IT, orders);

  EXPECT_THAT(orders.spawns(), SizeIs(1));
}

/**
 * Given a narrow enemy at x 2224, listed first, and a wide one at x 2300
 * whose left edge comes in earlier
 * When both are due
 * Then the wide one is asked for first
 */
TEST(PlacedEnemySpawner, SpawnsTheEnemiesInTheOrderTheyComeIn) {
  const std::array placements{
      placedEnemyOf(PLACED_DRIFTER_SHEET, PAST_THE_FIRST_SCREEN,
                    PLACED_ENEMY_WIDTH),
      placedEnemyOf(PLACED_DRIFTER_SHEET, WIDE_ENEMY_CENTER, WIDE_WIDTH)};
  PlacedEnemySpawner spawner{placements};
  RecordingEntityOrders orders{SPAWNED_ENEMY_ID};

  spawner.spawnDue(SCROLL_PAST_IT, orders);

  ASSERT_THAT(orders.spawns(), SizeIs(2));
  EXPECT_FLOAT_EQ(orders.spawns().front().position.y, WIDE_ENEMY_CENTER.y);
}

/**
 * Given two enemies coming in at 100 and 300 units of scroll, both spawned
 * When the level restarts at 200 units and scrolls past both again
 * Then only the one coming in at 300 is asked for again
 */
TEST(PlacedEnemySpawner, ARestartSkipsTheEnemiesThatCameInBefore) {
  const std::array placements{
      placedEnemyOf(PLACED_DRIFTER_SHEET, COMES_IN_AT_100, PLACED_ENEMY_WIDTH),
      placedEnemyOf(PLACED_DRIFTER_SHEET, COMES_IN_AT_300, PLACED_ENEMY_WIDTH)};
  PlacedEnemySpawner spawner{placements};
  RecordingEntityOrders orders{SPAWNED_ENEMY_ID};
  spawner.spawnDue(SCROLL_PAST_BOTH, orders);

  spawner.restartAt(CHECKPOINT_SCROLL);
  spawner.spawnDue(SCROLL_PAST_BOTH, orders);

  ASSERT_THAT(orders.spawns(), SizeIs(3));
  EXPECT_FLOAT_EQ(orders.spawns().back().position.y, COMES_IN_AT_300.y);
}

/**
 * Given an enemy coming in exactly at the checkpoint, 200 units
 * When the level restarts at 200 units and scrolls on
 * Then it is not asked for again: it had come in at the checkpoint
 */
TEST(PlacedEnemySpawner, ARestartSkipsAnEnemyThatCameInAtTheCheckpoint) {
  const std::array placements{placedEnemyOf(
      PLACED_DRIFTER_SHEET, COMES_IN_AT_THE_CHECKPOINT, PLACED_ENEMY_WIDTH)};
  PlacedEnemySpawner spawner{placements};
  RecordingEntityOrders orders{SPAWNED_ENEMY_ID};

  spawner.restartAt(CHECKPOINT_SCROLL);
  spawner.spawnDue(SCROLL_PAST_BOTH, orders);

  EXPECT_THAT(orders.spawns(), IsEmpty());
}

/**
 * Given an enemy coming in one tick of scroll after the checkpoint, 202
 * When the level restarts at 200 units and scrolls on
 * Then it is asked for again
 */
TEST(PlacedEnemySpawner, ARestartKeepsAnEnemyThatComesInRightAfterIt) {
  const std::array placements{placedEnemyOf(
      PLACED_DRIFTER_SHEET, COMES_IN_ONE_TICK_AFTER_THE_CHECKPOINT,
      PLACED_ENEMY_WIDTH)};
  PlacedEnemySpawner spawner{placements};
  RecordingEntityOrders orders{SPAWNED_ENEMY_ID};

  spawner.restartAt(CHECKPOINT_SCROLL);
  spawner.spawnDue(SCROLL_PAST_BOTH, orders);

  EXPECT_THAT(orders.spawns(), SizeIs(1));
}
