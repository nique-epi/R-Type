#include <gtest/gtest.h>
#include <vector>
#include "BossFight.hpp"
#include "BossTally.hpp"
#include "BossTallyFixtures.hpp"
#include "BossTallyTestConstants.hpp"
#include "BotTallyFixtures.hpp"
#include "TimeConstants.hpp"

using rtype::tools::BossFight;
using rtype::tools::BossTally;
using rtype::tools::testing::A_SECOND;
using rtype::tools::testing::ALIVE_NOTHING_LOST;
using rtype::tools::testing::ALIVE_TWO_LOST;
using rtype::tools::testing::bossArriving;
using rtype::tools::testing::bossDefeated;
using rtype::tools::testing::bossEnraged;
using rtype::tools::testing::bossFighting;
using rtype::tools::testing::DEAD_THREE_LOST;
using rtype::tools::testing::FULL_BOSS_HEALTH;
using rtype::tools::testing::gameEnded;
using rtype::tools::testing::HALF_A_SECOND;
using rtype::tools::testing::LAST_HEALTH_POINT;
using rtype::tools::testing::LAST_HEALTH_POINT_PERCENT;
using rtype::tools::testing::THREE_QUARTERS_OF_THE_HEALTH;
using rtype::tools::testing::THREE_QUARTERS_PERCENT;
using rtype::tools::testing::TWO_FIGHTS;
using rtype::tools::testing::TWO_LIVES;
using rtype::tools::testing::TWO_SECONDS;

/**
 * Given a tally that saw a boss arrive and never fight
 * When the game ends
 * Then it holds no fight
 */
TEST(BossTally, ABossThatOnlyArrivedIsNoFight) {
  BossTally tally;
  tally.count(bossArriving());

  tally.count(gameEnded());

  EXPECT_TRUE(tally.fights().empty());
}

/**
 * Given a tally that saw a boss fight
 * When the game ends with the boss alive
 * Then it holds one fight, not defeated
 */
TEST(BossTally, AFightEndsWithTheGame) {
  BossTally tally;
  tally.count(bossFighting(FULL_BOSS_HEALTH));

  tally.count(gameEnded());

  const std::vector<BossFight> fights = tally.fights();
  ASSERT_EQ(fights.size(), 1U);
  EXPECT_FALSE(fights.front().defeated);
}

/**
 * Given a tally that saw a boss fight and fall
 * When the game ends
 * Then its fight is defeated, the boss at 0 % health
 */
TEST(BossTally, ADefeatedBossMarksItsFight) {
  BossTally tally;
  tally.count(bossFighting(FULL_BOSS_HEALTH));
  tally.count(bossDefeated());

  tally.count(gameEnded());

  const std::vector<BossFight> fights = tally.fights();
  ASSERT_EQ(fights.size(), 1U);
  EXPECT_TRUE(fights.front().defeated);
  EXPECT_EQ(fights.front().bossHealthLeftPercent, 0U);
}

/**
 * Given a tally that saw a boss fight, then enrage with three quarters of its
 * health left
 * When the game ends
 * Then the boss kept 75 % of its health
 */
TEST(BossTally, TheHealthLeftIsThatOfTheNewestStatus) {
  BossTally tally;
  tally.count(bossFighting(FULL_BOSS_HEALTH));
  tally.count(bossEnraged(THREE_QUARTERS_OF_THE_HEALTH));

  tally.count(gameEnded());

  EXPECT_EQ(tally.fights().front().bossHealthLeftPercent,
            THREE_QUARTERS_PERCENT);
}

/**
 * Given a tally that saw a boss fight with a single health point left
 * When the game ends
 * Then the boss kept 1 %, never 0 for a boss still alive
 */
TEST(BossTally, ABossStillAliveNeverReadsZeroPercent) {
  BossTally tally;
  tally.count(bossFighting(LAST_HEALTH_POINT));

  tally.count(gameEnded());

  EXPECT_EQ(tally.fights().front().bossHealthLeftPercent,
            LAST_HEALTH_POINT_PERCENT);
}

/**
 * Given a tally that saw a boss arrive, with the bot alive
 * When a second of frames passes, then the boss fights and a second more passes
 * Then the fight holds the second spent after the boss fought, not the first
 */
TEST(BossTally, TimeCountsOnlyOnceTheBossFights) {
  BossTally tally;
  tally.count(bossArriving());
  tally.addTime(A_SECOND, ALIVE_NOTHING_LOST);
  tally.count(bossFighting(FULL_BOSS_HEALTH));
  tally.addTime(A_SECOND, ALIVE_NOTHING_LOST);

  tally.count(gameEnded());

  EXPECT_EQ(tally.fights().front().timeAlive, A_SECOND);
}

/**
 * Given a tally of a fight in which the bot lost its last life
 * When a second passes with the bot out of play
 * Then that second is not time alive
 */
TEST(BossTally, TimeStopsWhenTheBotHasNoLifeLeft) {
  BossTally tally;
  tally.count(bossFighting(FULL_BOSS_HEALTH));
  tally.addTime(HALF_A_SECOND, ALIVE_NOTHING_LOST);
  tally.addTime(A_SECOND, DEAD_THREE_LOST);

  tally.count(gameEnded());

  EXPECT_EQ(tally.fights().front().timeAlive, HALF_A_SECOND);
}

/**
 * Given a tally of a fight in which the boss fell
 * When two seconds pass with the bot alive
 * Then they are not time alive facing the boss
 */
TEST(BossTally, TimeStopsOnceTheBossIsDefeated) {
  BossTally tally;
  tally.count(bossFighting(FULL_BOSS_HEALTH));
  tally.addTime(A_SECOND, ALIVE_NOTHING_LOST);
  tally.count(bossDefeated());
  tally.addTime(TWO_SECONDS, ALIVE_NOTHING_LOST);

  tally.count(gameEnded());

  EXPECT_EQ(tally.fights().front().timeAlive, A_SECOND);
}

/**
 * Given a tally that read a bot with no life lost, before the boss fights
 * When the boss fights and the bot reads two lives lost
 * Then the fight holds the two lives lost
 */
TEST(BossTally, LivesLostWhileTheBossFightsAreCounted) {
  BossTally tally;
  tally.addTime(A_SECOND, ALIVE_NOTHING_LOST);
  tally.count(bossFighting(FULL_BOSS_HEALTH));
  tally.addTime(A_SECOND, ALIVE_TWO_LOST);

  tally.count(gameEnded());

  EXPECT_EQ(tally.fights().front().livesLost, TWO_LIVES);
}

/**
 * Given a tally that read two lives lost before the boss fights
 * When the boss fights and the bot loses no more
 * Then the fight holds no life lost
 */
TEST(BossTally, LivesLostBeforeTheBossFightsAreNotCounted) {
  BossTally tally;
  tally.addTime(A_SECOND, ALIVE_TWO_LOST);
  tally.count(bossFighting(FULL_BOSS_HEALTH));
  tally.addTime(A_SECOND, ALIVE_TWO_LOST);

  tally.count(gameEnded());

  EXPECT_EQ(tally.fights().front().livesLost, 0U);
}

/**
 * Given a tally that saw a boss fight and no end of its game yet
 * When it reports its fights
 * Then the fight still running is not in them
 */
TEST(BossTally, AFightStillRunningIsNotReported) {
  BossTally tally;
  tally.count(bossFighting(FULL_BOSS_HEALTH));

  EXPECT_TRUE(tally.fights().empty());
}

/**
 * Given a tally that saw a fight end with its game
 * When a second game's boss fights and that game ends too
 * Then it holds two fights
 */
TEST(BossTally, EachGameWhereTheBossFoughtMakesOneFight) {
  BossTally tally;
  tally.count(bossFighting(FULL_BOSS_HEALTH));
  tally.count(gameEnded());
  tally.count(bossFighting(FULL_BOSS_HEALTH));

  tally.count(gameEnded());

  EXPECT_EQ(tally.fights().size(), TWO_FIGHTS);
}
