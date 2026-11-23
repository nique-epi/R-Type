#include <gtest/gtest.h>
#include "BossHealth.hpp"
#include "BossHealthScaling.hpp"
#include "BossHealthScalingTestConstants.hpp"

using rtype::game::BossHealthScaling;
using rtype::game::scaledBossHealth;
using rtype::game::testing::BOSS_HEALTH_AT_HALF_THE_MOST_THREAT;
using rtype::game::testing::BOSS_HEALTH_AT_THE_MOST_THREAT;
using rtype::game::testing::BOSS_HEALTH_FOR_FOUR_PLAYERS;
using rtype::game::testing::BOSS_HEALTH_FOR_ONE_PLAYER;
using rtype::game::testing::BOSS_HEALTH_FOR_THREE_PLAYERS;
using rtype::game::testing::BOSS_HEALTH_FOR_TWO_PLAYERS;
using rtype::game::testing::FOUR_PLAYERS;
using rtype::game::testing::HALF_THE_MOST_THREAT_PERCENT;
using rtype::game::testing::HARD_BOSS_HEALTH_FOR_FOUR_PLAYERS;
using rtype::game::testing::HARD_BOSS_HEALTH_FOR_ONE_PLAYER;
using rtype::game::testing::HARD_HEALTH_PERCENT;
using rtype::game::testing::NO_PLAYER;
using rtype::game::testing::ONE_PLAYER;
using rtype::game::testing::PLAIN_HEALTH_PERCENT;
using rtype::game::testing::SHEET_BOSS_HEALTH;
using rtype::game::testing::THE_MOST_THREAT_PERCENT;
using rtype::game::testing::THREE_PLAYERS;
using rtype::game::testing::TWO_PLAYERS;

/**
 * Given the 240 points of the sheet of the boss and a difficulty at 100 %
 * When one to four players face it
 * Then it has 240, 420, 600 and 780 points: 75 % more per player past one
 */
TEST(BossHealthScaling, EachPlayerPastTheFirstAddsThreeQuarters) {
  EXPECT_EQ(scaledBossHealth(
                BossHealthScaling{.points = SHEET_BOSS_HEALTH,
                                  .players = ONE_PLAYER,
                                  .healthPercent = PLAIN_HEALTH_PERCENT}),
            BOSS_HEALTH_FOR_ONE_PLAYER);
  EXPECT_EQ(scaledBossHealth(
                BossHealthScaling{.points = SHEET_BOSS_HEALTH,
                                  .players = TWO_PLAYERS,
                                  .healthPercent = PLAIN_HEALTH_PERCENT}),
            BOSS_HEALTH_FOR_TWO_PLAYERS);
  EXPECT_EQ(scaledBossHealth(
                BossHealthScaling{.points = SHEET_BOSS_HEALTH,
                                  .players = THREE_PLAYERS,
                                  .healthPercent = PLAIN_HEALTH_PERCENT}),
            BOSS_HEALTH_FOR_THREE_PLAYERS);
  EXPECT_EQ(scaledBossHealth(
                BossHealthScaling{.points = SHEET_BOSS_HEALTH,
                                  .players = FOUR_PLAYERS,
                                  .healthPercent = PLAIN_HEALTH_PERCENT}),
            BOSS_HEALTH_FOR_FOUR_PLAYERS);
}

/**
 * Given the 240 points of the sheet and the 130 % of the hard difficulty
 * When one player, then four, face the boss
 * Then it has 312 points, then 1014: the difficulty scales what the players
 * already scaled
 */
TEST(BossHealthScaling, TheDifficultyScalesTheHealthOfThePlayers) {
  EXPECT_EQ(
      scaledBossHealth(BossHealthScaling{.points = SHEET_BOSS_HEALTH,
                                         .players = ONE_PLAYER,
                                         .healthPercent = HARD_HEALTH_PERCENT}),
      HARD_BOSS_HEALTH_FOR_ONE_PLAYER);
  EXPECT_EQ(
      scaledBossHealth(BossHealthScaling{.points = SHEET_BOSS_HEALTH,
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
                BossHealthScaling{.points = SHEET_BOSS_HEALTH,
                                  .players = NO_PLAYER,
                                  .healthPercent = PLAIN_HEALTH_PERCENT}),
            BOSS_HEALTH_FOR_ONE_PLAYER);
}

/**
 * Given the 240 points of the sheet, one player and a difficulty at 100 %
 * When the boss arrives at the most threat
 * Then it has 600 points: 150 % more
 */
TEST(BossHealthScaling, TheMostThreatAddsOneAndAHalf) {
  EXPECT_EQ(scaledBossHealth(
                BossHealthScaling{.points = SHEET_BOSS_HEALTH,
                                  .players = ONE_PLAYER,
                                  .healthPercent = PLAIN_HEALTH_PERCENT,
                                  .threatPercent = THE_MOST_THREAT_PERCENT}),
            BOSS_HEALTH_AT_THE_MOST_THREAT);
}

/**
 * Given the 240 points of the sheet, one player and a difficulty at 100 %
 * When the boss arrives at half the most threat
 * Then it has 420 points: half of 150 % more
 */
TEST(BossHealthScaling, HalfTheMostThreatAddsThreeQuarters) {
  EXPECT_EQ(scaledBossHealth(BossHealthScaling{
                .points = SHEET_BOSS_HEALTH,
                .players = ONE_PLAYER,
                .healthPercent = PLAIN_HEALTH_PERCENT,
                .threatPercent = HALF_THE_MOST_THREAT_PERCENT}),
            BOSS_HEALTH_AT_HALF_THE_MOST_THREAT);
}
