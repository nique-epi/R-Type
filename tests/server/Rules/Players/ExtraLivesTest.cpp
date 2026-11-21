#include <gtest/gtest.h>
#include "ExtraLives.hpp"
#include "ExtraLivesTestConstants.hpp"
#include "ScoreChanged.hpp"

namespace rtype::server::testing {

/**
 * Given a score that goes from 29999 to exactly 30000
 * When the extra lives it wins are counted
 * Then it wins one: the first step is reached
 */
TEST(ExtraLives, ReachingTheFirstStepWinsALife) {
  EXPECT_EQ(
      extraLivesWonBy(game::ScoreChanged{.before = JUST_UNDER_THE_FIRST_STEP,
                                         .after = AT_THE_FIRST_STEP}),
      1U);
}

/**
 * Given a score that stays under the first step
 * When the extra lives it wins are counted
 * Then it wins none
 */
TEST(ExtraLives, AScoreUnderTheFirstStepWinsNothing) {
  EXPECT_EQ(extraLivesWonBy(game::ScoreChanged{
                .before = NO_SCORE, .after = JUST_UNDER_THE_FIRST_STEP}),
            0U);
}

/**
 * Given a score already at the first step that grows a little past it
 * When the extra lives it wins are counted
 * Then it wins none: the first step was won before
 */
TEST(ExtraLives, AStepIsWonOnce) {
  EXPECT_EQ(extraLivesWonBy(game::ScoreChanged{.before = AT_THE_FIRST_STEP,
                                               .after = PAST_THE_FIRST_STEP}),
            0U);
}

/**
 * Given a score that jumps from 29999 past 150000
 * When the extra lives it wins are counted
 * Then it wins three: 30000, 90000 and 150000
 */
TEST(ExtraLives, OneKillCanPassSeveralSteps) {
  EXPECT_EQ(
      extraLivesWonBy(game::ScoreChanged{.before = JUST_UNDER_THE_FIRST_STEP,
                                         .after = PAST_THE_THIRD_STEP}),
      3U);
}

/**
 * Given a score just past the first step that reaches 90000
 * When the extra lives it wins are counted
 * Then it wins the second step only
 */
TEST(ExtraLives, TheStepsComeEverySixtyThousand) {
  EXPECT_EQ(extraLivesWonBy(game::ScoreChanged{.before = PAST_THE_FIRST_STEP,
                                               .after = AT_THE_SECOND_STEP}),
            1U);
}

/**
 * Given a score that did not grow
 * When the extra lives it wins are counted
 * Then it wins none
 */
TEST(ExtraLives, AScoreThatDidNotGrowWinsNothing) {
  EXPECT_EQ(extraLivesWonBy(game::ScoreChanged{.before = AT_THE_SECOND_STEP,
                                               .after = AT_THE_FIRST_STEP}),
            0U);
}

}  // namespace rtype::server::testing
