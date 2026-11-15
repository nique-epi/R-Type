#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <vector>
#include "BossArrival.hpp"
#include "BossProgress.hpp"
#include "BossSystemArrivalFixture.hpp"
#include "BossSystemArrivalTestConstants.hpp"

using rtype::game::BossArrival;
using rtype::game::BossProgress;
using rtype::game::testing::ARRIVING_BOSS_HEALTH;
using rtype::game::testing::ARRIVING_BOSS_HEALTH_FOR_THREE;
using rtype::game::testing::ARRIVING_BOSS_HEALTH_WHEN_HARD;
using rtype::game::testing::ARRIVING_BOSS_SHEET_NUMBER;
using rtype::game::testing::BossSystemArrivalFixture;
using rtype::game::testing::HARD_DIFFICULTY_HEALTH_PERCENT;
using rtype::game::testing::MILLISECONDS_OF_A_BOSS_WARNING;
using rtype::game::testing::THREE_CONNECTED_PILOTS;
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
  connect(THREE_CONNECTED_PILOTS);
  addNewBoss();
  run();

  run();

  EXPECT_EQ(health().full, ARRIVING_BOSS_HEALTH);
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
  connect(THREE_CONNECTED_PILOTS);
  addNewBoss();
  run();
  endTheWarning();

  run();

  EXPECT_EQ(health().full, ARRIVING_BOSS_HEALTH_FOR_THREE);
  EXPECT_EQ(health().points, ARRIVING_BOSS_HEALTH_FOR_THREE);
}

/**
 * Given a difficulty that gives bosses 130 percent of their health, and a
 * boss whose warning just ended
 * When the boss system runs
 * Then the boss has 312 points
 */
TEST_F(BossSystemArrivalFixture, ScalesTheHealthForTheDifficulty) {
  useHealthPercent(HARD_DIFFICULTY_HEALTH_PERCENT);
  addNewBoss();
  run();
  endTheWarning();

  run();

  EXPECT_EQ(health().full, ARRIVING_BOSS_HEALTH_WHEN_HARD);
}

/**
 * Given three connected players and a boss scaled as its warning ended
 * When the boss system runs again
 * Then the boss still has 360 points: its health is scaled once
 */
TEST_F(BossSystemArrivalFixture, ScalesTheHealthOnce) {
  connect(THREE_CONNECTED_PILOTS);
  addNewBoss();
  run();
  endTheWarning();
  run();

  run();

  EXPECT_EQ(health().full, ARRIVING_BOSS_HEALTH_FOR_THREE);
}
