#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "BossStageTestConstants.hpp"
#include "LevelBossesFixture.hpp"
#include "LevelBossesTestConstants.hpp"

using ::testing::ElementsAreArray;
using ::testing::SizeIs;

namespace rtype::server::testing {

/**
 * Given the bosses of a level
 * When a stage names the warlord
 * Then its arrival is announced at once
 */
TEST_F(LevelBossesFixture, AnnouncesTheBossAtOnce) {
  bosses().announce(WARLORD_NAME_IN_TESTS, WHOLE_WARLORD_PERCENT);

  EXPECT_THAT(arrivals(), SizeIs(ONE_BOSS));
}

/**
 * Given the warlord announced
 * When the ticks of its warning pass
 * Then it is still out of the world, and still pending
 */
TEST_F(LevelBossesFixture, KeepsTheBossOutDuringItsWarning) {
  bosses().announce(WARLORD_NAME_IN_TESTS, WHOLE_WARLORD_PERCENT);

  tick(TICKS_OF_THE_WARNING);

  EXPECT_THAT(bossHealths(), SizeIs(NO_BOSS));
  EXPECT_EQ(bosses().pending(), ONE_BOSS);
}

/**
 * Given the warlord announced
 * When the tick after its warning passes
 * Then it is in the world, and no boss is pending
 */
TEST_F(LevelBossesFixture, BringsTheBossInOnceItsWarningIsOver) {
  bosses().announce(WARLORD_NAME_IN_TESTS, WHOLE_WARLORD_PERCENT);

  tick(TICKS_UNTIL_THE_BOSS_ENTERS);

  EXPECT_THAT(bossHealths(), SizeIs(ONE_BOSS));
  EXPECT_EQ(bosses().pending(), NO_BOSS);
}

/**
 * Given a warlord announced, then another five ticks later
 * When the warning of the first is over
 * Then only the first entered; the second enters five ticks later
 */
TEST_F(LevelBossesFixture, BringsEachBossInAtItsOwnTime) {
  bosses().announce(WARLORD_NAME_IN_TESTS, WHOLE_WARLORD_PERCENT);
  tick(TICKS_BETWEEN_TWO_BOSSES);
  bosses().announce(WARLORD_NAME_IN_TESTS, WHOLE_WARLORD_PERCENT);

  tick(TICKS_UNTIL_THE_BOSS_ENTERS - TICKS_BETWEEN_TWO_BOSSES);
  EXPECT_THAT(bossHealths(), SizeIs(ONE_BOSS));
  tick(TICKS_BETWEEN_TWO_BOSSES);

  EXPECT_THAT(bossHealths(), SizeIs(TWO_BOSSES));
}

/**
 * Given the warlord announced, then forgotten as a restart does
 * When its warning would be over
 * Then it never enters
 */
TEST_F(LevelBossesFixture, ForgetsTheBossesThatHaveNotEntered) {
  bosses().announce(WARLORD_NAME_IN_TESTS, WHOLE_WARLORD_PERCENT);

  bosses().forget();
  tick(TICKS_UNTIL_THE_BOSS_ENTERS);

  EXPECT_THAT(bossHealths(), SizeIs(NO_BOSS));
}

/**
 * Given a warlord of 20 points, in the world with its whole health
 * When a second warlord enters with half its health
 * Then the second has 10 points, and the first keeps its 20
 */
TEST_F(LevelBossesFixture, ScalesTheHealthOfTheBossThatEntersAlone) {
  bosses().announce(WARLORD_NAME_IN_TESTS, WHOLE_WARLORD_PERCENT);
  tick(TICKS_UNTIL_THE_BOSS_ENTERS);

  bosses().announce(WARLORD_NAME_IN_TESTS, HALF_WARLORD_PERCENT);
  tick(TICKS_UNTIL_THE_BOSS_ENTERS);

  EXPECT_THAT(bossHealths(),
              ElementsAreArray(HEALTHS_OF_A_WHOLE_THEN_A_HALF_WARLORD));
}

/**
 * Given a difficulty that halves the health of bosses
 * When a level brings a warlord in at half its health
 * Then it has a quarter of its 20 points
 */
TEST_F(LevelBossesFixture, TakesTheHealthOfTheDifficultyOnTopOfTheLevels) {
  playDifficulty(DIFFICULTY_OF_HALF_BOSSES);

  bosses().announce(WARLORD_NAME_IN_TESTS, HALF_WARLORD_PERCENT);
  tick(TICKS_UNTIL_THE_BOSS_ENTERS);

  EXPECT_THAT(bossHealths(), ElementsAreArray(HEALTH_OF_A_QUARTER_WARLORD));
}

/**
 * Given the sheets of the tests
 * When the warlord is asked about
 * Then it is a boss
 */
TEST_F(LevelBossesFixture, KnowsASheetWithABossTableAsABoss) {
  EXPECT_TRUE(bosses().isBoss(WARLORD_NAME_IN_TESTS));
}

/**
 * Given the sheets of the tests
 * When the minion is asked about
 * Then it is no boss
 */
TEST_F(LevelBossesFixture, KnowsASheetWithoutABossTableAsNoBoss) {
  EXPECT_FALSE(bosses().isBoss(STAGE_MINION_NAME_IN_TESTS));
}

/**
 * Given the sheets of the tests
 * When a name no sheet has is asked about
 * Then it is no boss
 */
TEST_F(LevelBossesFixture, KnowsANameWithoutSheetAsNoBoss) {
  EXPECT_FALSE(bosses().isBoss(NO_SHEET_NAME));
}

}  // namespace rtype::server::testing
