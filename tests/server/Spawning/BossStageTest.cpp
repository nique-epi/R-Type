#include <gtest/gtest.h>
#include <optional>
#include "BossStageFixture.hpp"
#include "BossStageTestConstants.hpp"
#include "PercentConstants.hpp"
#include "RecordingEntityOrders.hpp"

using rtype::game::WHOLE_PERCENT;
using rtype::server::testing::A_FEW_STAGE_TICKS;
using rtype::server::testing::BossStageFixture;
using rtype::server::testing::TICKS_TO_ORDER_THE_BOSS;
using rtype::server::testing::WARLORD_NAME_IN_TESTS;
using rtype::server::testing::WHERE_THE_BOSS_IS_ORDERED;

/**
 * Given a stage whose level ends with the warlord
 * When ticks run while the waves go on
 * Then no boss is ordered
 */
TEST_F(BossStageFixture, OrdersNothingWhileTheWavesGoOn) {
  runTicks(A_FEW_STAGE_TICKS, false);

  EXPECT_TRUE(orders().empty());
}

/**
 * Given a stage whose level ends with the warlord
 * When the first tick that finds the last wave over runs
 * Then the warlord is ordered, half its frame past the right edge, with the
 * whole of its health
 */
TEST_F(BossStageFixture, OrdersTheBossOnceTheLastWaveIsOver) {
  runTicks(TICKS_TO_ORDER_THE_BOSS, true);

  ASSERT_EQ(orders().size(), 1U);
  const RecordedSpawn& order = orders().front();
  EXPECT_EQ(order.type, WARLORD_NAME_IN_TESTS);
  EXPECT_EQ(order.position.x, WHERE_THE_BOSS_IS_ORDERED.x);
  EXPECT_EQ(order.position.y, WHERE_THE_BOSS_IS_ORDERED.y);
  EXPECT_EQ(order.options.healthPercent, WHOLE_PERCENT);
}

/**
 * Given a stage that ordered the warlord
 * When more ticks run with the last wave over
 * Then the warlord is ordered once
 */
TEST_F(BossStageFixture, OrdersTheBossOnce) {
  runTicks(TICKS_TO_ORDER_THE_BOSS, true);
  addBoss();

  runTicks(A_FEW_STAGE_TICKS, true);

  EXPECT_EQ(orders().size(), 1U);
}

/**
 * Given a stage whose level ends with the warlord, in a world without any
 * enemy
 * When the tick that orders the warlord runs
 * Then the level is not cleared: its boss is yet to come
 */
TEST_F(BossStageFixture, IsNotClearedOnTheTickItOrdersTheBoss) {
  runTicks(TICKS_TO_ORDER_THE_BOSS, true);

  EXPECT_FALSE(cleared());
}

/**
 * Given the warlord in play
 * When ticks run
 * Then the level is not cleared
 */
TEST_F(BossStageFixture, IsNotClearedWhileTheBossLives) {
  runTicks(TICKS_TO_ORDER_THE_BOSS, true);
  addBoss();

  runTicks(A_FEW_STAGE_TICKS, true);

  EXPECT_FALSE(cleared());
}

/**
 * Given the warlord in play, then killed
 * When one tick runs
 * Then the level is cleared: no enemy is left
 */
TEST_F(BossStageFixture, IsClearedOnceNoEnemyIsLeft) {
  runTicks(TICKS_TO_ORDER_THE_BOSS, true);
  addBoss();
  runTicks(A_FEW_STAGE_TICKS, true);
  destroyTheBosses();

  runTicks(1, true);

  EXPECT_TRUE(cleared());
}

/**
 * Given the warlord in play with a minion, then the warlord killed
 * When one tick runs
 * Then the level is not cleared: the minion is an enemy left
 */
TEST_F(BossStageFixture, IsClearedOnlyOnceAMinionIsGoneToo) {
  runTicks(TICKS_TO_ORDER_THE_BOSS, true);
  addBoss();
  addMinion();
  destroyTheBosses();

  runTicks(1, true);

  EXPECT_FALSE(cleared());
}

/**
 * Given a stage whose level ends with its last wave, with no boss
 * When the first tick that finds the last wave over runs
 * Then the level is cleared on that tick
 */
TEST_F(BossStageFixture, IsClearedOnTheTickTheLastWaveIsOver) {
  buildStage(std::nullopt);

  runTicks(1, true);

  EXPECT_TRUE(cleared());
}

/**
 * Given a stage whose level ends with its last wave, with no boss
 * When ticks run with the last wave over
 * Then no boss is ordered
 */
TEST_F(BossStageFixture, OrdersNoBossForALevelWithoutOne) {
  buildStage(std::nullopt);

  runTicks(A_FEW_STAGE_TICKS, true);

  EXPECT_TRUE(orders().empty());
}

/**
 * Given a stage whose boss the game refuses to create
 * When the tick that orders it, then one more, run in a world without enemy
 * Then the level is cleared: the game goes on without its boss
 */
TEST_F(BossStageFixture, IsClearedWithoutEnemyWhenTheBossIsRefused) {
  refuseTheBoss();

  runTicks(TICKS_TO_ORDER_THE_BOSS + 1, true);

  EXPECT_TRUE(cleared());
}
