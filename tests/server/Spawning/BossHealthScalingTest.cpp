#include <gtest/gtest.h>
#include "BossEntrance.hpp"
#include "BossHealthScaling.hpp"
#include "BossHealthScalingTestConstants.hpp"

using rtype::server::BossHealthScaling;
using rtype::server::scaledBossHealth;
using rtype::server::testing::BOSS_HEALTH_FOR_FOUR_PLAYERS;
using rtype::server::testing::BOSS_HEALTH_FOR_ONE_PLAYER;
using rtype::server::testing::BOSS_HEALTH_FOR_THREE_PLAYERS;
using rtype::server::testing::BOSS_HEALTH_FOR_TWO_PLAYERS;
using rtype::server::testing::FOUR_PLAYERS;
using rtype::server::testing::HARD_BOSS_HEALTH_FOR_FOUR_PLAYERS;
using rtype::server::testing::HARD_BOSS_HEALTH_FOR_ONE_PLAYER;
using rtype::server::testing::HARD_HEALTH_PERCENT;
using rtype::server::testing::NO_PLAYER;
using rtype::server::testing::ONE_PLAYER;
using rtype::server::testing::PLAIN_HEALTH_PERCENT;
using rtype::server::testing::SHEET_BOSS_HEALTH;
using rtype::server::testing::THREE_PLAYERS;
using rtype::server::testing::TWO_PLAYERS;

/**
 * Given the 240 points of the sheet of the boss and a difficulty at 100 %
 * When one to four players face it
 * Then it has 240, 300, 360 and 420 points: 25 % more per player past one
 */
TEST(BossHealthScaling, EachPlayerPastTheFirstAddsAQuarter) {
  EXPECT_EQ(scaledBossHealth(
                BossHealthScaling{.sheetPoints = SHEET_BOSS_HEALTH,
                                  .players = ONE_PLAYER,
                                  .healthPercent = PLAIN_HEALTH_PERCENT}),
            BOSS_HEALTH_FOR_ONE_PLAYER);
  EXPECT_EQ(scaledBossHealth(
                BossHealthScaling{.sheetPoints = SHEET_BOSS_HEALTH,
                                  .players = TWO_PLAYERS,
                                  .healthPercent = PLAIN_HEALTH_PERCENT}),
            BOSS_HEALTH_FOR_TWO_PLAYERS);
  EXPECT_EQ(scaledBossHealth(
                BossHealthScaling{.sheetPoints = SHEET_BOSS_HEALTH,
                                  .players = THREE_PLAYERS,
                                  .healthPercent = PLAIN_HEALTH_PERCENT}),
            BOSS_HEALTH_FOR_THREE_PLAYERS);
  EXPECT_EQ(scaledBossHealth(
                BossHealthScaling{.sheetPoints = SHEET_BOSS_HEALTH,
                                  .players = FOUR_PLAYERS,
                                  .healthPercent = PLAIN_HEALTH_PERCENT}),
            BOSS_HEALTH_FOR_FOUR_PLAYERS);
}

/**
 * Given the 240 points of the sheet and the 130 % of the hard difficulty
 * When one player, then four, face the boss
 * Then it has 312 points, then 546: the difficulty scales what the players
 * already scaled
 */
TEST(BossHealthScaling, TheDifficultyScalesTheHealthOfThePlayers) {
  EXPECT_EQ(
      scaledBossHealth(BossHealthScaling{.sheetPoints = SHEET_BOSS_HEALTH,
                                         .players = ONE_PLAYER,
                                         .healthPercent = HARD_HEALTH_PERCENT}),
      HARD_BOSS_HEALTH_FOR_ONE_PLAYER);
  EXPECT_EQ(
      scaledBossHealth(BossHealthScaling{.sheetPoints = SHEET_BOSS_HEALTH,
                                         .players = FOUR_PLAYERS,
                                         .healthPercent = HARD_HEALTH_PERCENT}),
      HARD_BOSS_HEALTH_FOR_FOUR_PLAYERS);
}

/**
 * Given the 240 points of the sheet and a difficulty at 100 %
 * When no player is connected as the boss arrives
 * Then it has the 240 points of one player, never fewer
 */
TEST(BossHealthScaling, NoPlayerCountsAsOne) {
  EXPECT_EQ(scaledBossHealth(
                BossHealthScaling{.sheetPoints = SHEET_BOSS_HEALTH,
                                  .players = NO_PLAYER,
                                  .healthPercent = PLAIN_HEALTH_PERCENT}),
            BOSS_HEALTH_FOR_ONE_PLAYER);
}
