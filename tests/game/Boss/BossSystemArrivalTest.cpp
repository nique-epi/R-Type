#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <vector>
#include "BossArrival.hpp"
#include "BossHealthScalingTestConstants.hpp"
#include "BossProgress.hpp"
#include "BossSystemArrivalFixture.hpp"
#include "BossSystemArrivalTestConstants.hpp"

using rtype::game::BossArrival;
using rtype::game::BossProgress;
using rtype::game::testing::ARRIVING_BOSS_SHEET_NUMBER;
using rtype::game::testing::BOSS_HEALTH_FOR_THREE_PLAYERS;
using rtype::game::testing::BossSystemArrivalFixture;
using rtype::game::testing::HARD_BOSS_HEALTH_FOR_ONE_PLAYER;
using rtype::game::testing::HARD_HEALTH_PERCENT;
using rtype::game::testing::MILLISECONDS_OF_A_BOSS_WARNING;
using rtype::game::testing::SHEET_BOSS_HEALTH;
using rtype::game::testing::THREE_PLAYERS;
using rtype::game::testing::TICKS_OF_A_BOSS_WARNING;

/**
 * Given a boss just created
 * When the boss system runs
 * Then the players are warned of a boss of its sheet, for 4400 ms
 */
TEST_F(BossSystemArrivalFixture, WarnsThePlayersOfANewBoss) {
  addNewBoss();

  run();

  const std::vector<BossArrival> expected{
      {.typeNumber = ARRIVING_BOSS_SHEET_NUMBER,
       .warningMilliseconds = MILLISECONDS_OF_A_BOSS_WARNING}};
  EXPECT_EQ(arrivals(), expected);
}

/**
 * Given a boss just created
 * When the boss system runs twice
 * Then the players are warned of it once
 */
TEST_F(BossSystemArrivalFixture, WarnsOfABossOnce) {
  addNewBoss();

  run();
  run();

  EXPECT_EQ(arrivals().size(), 1U);
}

/**
 * Given a boss just created
 * When the boss system runs
 * Then the boss is summoned for the 264 ticks of its warning
 */
TEST_F(BossSystemArrivalFixture, KeepsANewBossSummonedForItsWarning) {
  addNewBoss();

  run();

  EXPECT_EQ(summoningTicksLeft(),
            std::optional<std::size_t>{TICKS_OF_A_BOSS_WARNING});
}

/**
 * Given a boss the players were warned of, still summoned
 * When the boss system runs again
 * Then the boss is still warned of, and nothing brings it in
 */
TEST_F(BossSystemArrivalFixture, WaitsWhileTheBossIsSummoned) {
  addNewBoss();
  run();

  run();

  EXPECT_EQ(progress(), BossProgress::Warned);
}

/**
 * Given three connected players and a boss still summoned for its warning
 * When the boss system runs
 * Then the boss keeps the 240 points it appeared with
 */
TEST_F(BossSystemArrivalFixture, LeavesTheHealthAloneDuringTheWarning) {
  connect(THREE_PLAYERS);
  addNewBoss();
  run();

  run();

  EXPECT_EQ(health().full, SHEET_BOSS_HEALTH);
}

/**
 * Given a boss whose warning just ended
 * When the boss system runs
 * Then the boss is arriving
 */
TEST_F(BossSystemArrivalFixture, LetsTheBossArriveOnceItsWarningIsOver) {
  addNewBoss();
  run();
  endTheWarning();

  run();

  EXPECT_EQ(progress(), BossProgress::Arriving);
}

/**
 * Given three connected players and a boss whose warning just ended
 * When the boss system runs
 * Then the boss has 360 points, all of them: a quarter more for each player
 * past the first
 */
TEST_F(BossSystemArrivalFixture, ScalesTheHealthForThePlayersAfterTheWarning) {
  connect(THREE_PLAYERS);
  addNewBoss();
  run();
  endTheWarning();

  run();

  EXPECT_EQ(health().full, BOSS_HEALTH_FOR_THREE_PLAYERS);
  EXPECT_EQ(health().points, BOSS_HEALTH_FOR_THREE_PLAYERS);
}

/**
 * Given a difficulty that gives bosses 130 percent of their health, and a
 * boss whose warning just ended
 * When the boss system runs
 * Then the boss has 312 points
 */
TEST_F(BossSystemArrivalFixture, ScalesTheHealthForTheDifficulty) {
  useHealthPercent(HARD_HEALTH_PERCENT);
  addNewBoss();
  run();
  endTheWarning();

  run();

  EXPECT_EQ(health().full, HARD_BOSS_HEALTH_FOR_ONE_PLAYER);
}

/**
 * Given three connected players and a boss scaled as its warning ended
 * When the boss system runs again
 * Then the boss still has 360 points: its health is scaled once
 */
TEST_F(BossSystemArrivalFixture, ScalesTheHealthOnce) {
  connect(THREE_PLAYERS);
  addNewBoss();
  run();
  endTheWarning();
  run();

  run();

  EXPECT_EQ(health().full, BOSS_HEALTH_FOR_THREE_PLAYERS);
}
