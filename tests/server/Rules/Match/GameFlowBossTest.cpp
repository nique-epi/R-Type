#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>
#include "BossProgress.hpp"
#include "GameEnded.hpp"
#include "GameFlowBossLevelFixture.hpp"
#include "GameFlowTestConstants.hpp"
#include "GameState.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "game/Waves/WaveTestConstants.hpp"

using rtype::game::BossProgress;
using rtype::game::testing::TICKS_TO_THE_WHOLE_FIRST_WAVE;
using rtype::network::GameEnded;
using rtype::network::WireGameOutcome;
using rtype::server::GameState;
using rtype::server::testing::A_SECOND_OF_TICKS;
using rtype::server::testing::FIRST_PLAYER;
using rtype::server::testing::GameFlowBossLevelFixture;
using rtype::server::testing::TICKS_FROM_THE_CLEAR_SKY_TO_THE_BOSS;
using rtype::server::testing::TICKS_FROM_THE_CLEAR_SKY_TO_THE_WARNING;
using rtype::server::testing::TICKS_TO_FIND_THE_SKY_CLEAR;
using ::testing::ElementsAre;

/**
 * Given a game on a level of one wave then a boss, every Bydo of the wave
 * killed
 * When the ticks that would end a level without boss run
 * Then the game goes on: the boss comes after the last wave
 */
TEST_F(GameFlowBossLevelFixture, PlaysOnWhenABossFollowsTheLastWave) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_TO_THE_WHOLE_FIRST_WAVE);
  clearTheBydos();

  runTicks(TICKS_TO_FIND_THE_SKY_CLEAR + 1);

  EXPECT_EQ(flow().state(), GameState::InProgress);
}

/**
 * Given a game on a level of one wave then a boss, every Bydo of the wave
 * killed
 * When the tick that finds the sky clear runs, then the next one
 * Then the boss is in the world
 */
TEST_F(GameFlowBossLevelFixture, CreatesTheBossOnceTheLastWaveIsOver) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_TO_THE_WHOLE_FIRST_WAVE);
  clearTheBydos();

  runTicks(TICKS_FROM_THE_CLEAR_SKY_TO_THE_BOSS);

  EXPECT_EQ(bossesInPlay(), 1U);
}

/**
 * Given a game on a level of one wave then a boss, every Bydo of the wave
 * killed
 * When the boss appeared and one more tick ran
 * Then the players were warned of it, as the systems of the game do for any
 * boss
 */
TEST_F(GameFlowBossLevelFixture, WarnsThePlayersOfTheBossOfTheWaves) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_TO_THE_WHOLE_FIRST_WAVE);
  clearTheBydos();

  runTicks(TICKS_FROM_THE_CLEAR_SKY_TO_THE_WARNING);

  EXPECT_THAT(bossProgress(), ElementsAre(BossProgress::Warned));
}

/**
 * Given the boss of the level in play
 * When a second of ticks runs
 * Then the game goes on: it is not won while the boss lives
 */
TEST_F(GameFlowBossLevelFixture, IsNotWonWhileTheBossLives) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_TO_THE_WHOLE_FIRST_WAVE);
  clearTheBydos();
  runTicks(TICKS_FROM_THE_CLEAR_SKY_TO_THE_BOSS);

  runTicks(A_SECOND_OF_TICKS);

  EXPECT_EQ(flow().state(), GameState::InProgress);
}

/**
 * Given the boss of the level in play
 * When every enemy is destroyed, the boss included, and the game finishes
 * Then the player is sent a GameEnded that says Victory
 */
TEST_F(GameFlowBossLevelFixture, IsWonOnceTheBossIsDestroyed) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_TO_THE_WHOLE_FIRST_WAVE);
  clearTheBydos();
  runTicks(TICKS_FROM_THE_CLEAR_SKY_TO_THE_BOSS);
  clearTheBydos();
  runTicks(TICKS_TO_FIND_THE_SKY_CLEAR + 1);

  runTicks(1);

  const std::vector<GameEnded> ends = endsSentTo(FIRST_PLAYER);
  ASSERT_FALSE(ends.empty());
  EXPECT_EQ(ends.front().outcome, WireGameOutcome::Victory);
}
