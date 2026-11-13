#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include "LevelCheckpoint.hpp"
#include "LuauLevelConstants.hpp"
#include "LuauLevelRunFixture.hpp"
#include "LuauLevelRunState.hpp"
#include "LuauLevelRunTestConstants.hpp"

using rtype::game::LevelCheckpoint;
using rtype::game::LuauLevelRunState;
using ::testing::HasSubstr;

/**
 * Given a level whose stage names a biome
 * When the first tick passes
 * Then the stage ran: the level shows that biome
 */
TEST_F(LuauLevelRunFixture, RunsItsFirstStageAtTheFirstTick) {
  play(NAMES_A_BIOME);

  tick(1);

  EXPECT_EQ(progress().biome, STATION_BIOME);
}

/**
 * Given a level whose only stage returns
 * When the tick that runs it passes
 * Then the level is cleared
 */
TEST_F(LuauLevelRunFixture, IsClearedOnceItsLastStageReturned) {
  play(NAMES_A_BIOME);

  tick(1);

  EXPECT_EQ(run().state(), LuauLevelRunState::Cleared);
}

/**
 * Given a level whose stage waits 3 ticks between two biomes
 * When the last tick of the wait passes
 * Then the level still shows the first biome, and is not cleared
 */
TEST_F(LuauLevelRunFixture, HoldsAStageUntilTheLastTickOfItsWait) {
  play(WAITS_BETWEEN_TWO_BIOMES);

  tick(TICKS_UNTIL_THE_LAST_OF_THE_WAIT);

  EXPECT_EQ(progress().biome, BIOME_BEFORE_THE_WAIT);
  EXPECT_EQ(run().state(), LuauLevelRunState::Running);
}

/**
 * Given a level whose stage waits 3 ticks between two biomes
 * When the tick after the wait passes
 * Then the stage went on: the level shows the second biome
 */
TEST_F(LuauLevelRunFixture, ResumesAStageOnceItsWaitIsOver) {
  play(WAITS_BETWEEN_TWO_BIOMES);

  tick(TICKS_UNTIL_THE_WAIT_IS_OVER);

  EXPECT_EQ(progress().biome, BIOME_AFTER_THE_WAIT);
}

/**
 * Given a level whose stage waits for one second of scroll
 * When the level is one tick short of it
 * Then the stage still waits
 */
TEST_F(LuauLevelRunFixture, HoldsAStageUntilTheLevelScrolledFarEnough) {
  play(WAITS_FOR_A_SECOND_OF_SCROLL);

  tick(TICKS_JUST_SHORT_OF_A_SECOND_OF_SCROLL);

  EXPECT_EQ(progress().biome, BIOME_BEFORE_THE_SCROLL);
}

/**
 * Given a level whose stage waits for one second of scroll
 * When the level scrolled that far
 * Then the stage goes on
 */
TEST_F(LuauLevelRunFixture, ResumesAStageOnceTheLevelScrolledFarEnough) {
  play(WAITS_FOR_A_SECOND_OF_SCROLL);

  tick(TICKS_JUST_SHORT_OF_A_SECOND_OF_SCROLL + 1);

  EXPECT_EQ(progress().biome, BIOME_AFTER_THE_SCROLL);
}

/**
 * Given a level whose stage names a biome once it scrolled 122 units
 * When the stage goes on
 * Then that biome began where the scroll stood
 */
TEST_F(LuauLevelRunFixture, BeginsABiomeWhereTheScrollStands) {
  play(WAITS_FOR_A_SECOND_OF_SCROLL);

  tick(TICKS_JUST_SHORT_OF_A_SECOND_OF_SCROLL + 1);

  EXPECT_NEAR(progress().biomeSince, WHERE_THE_BIOME_AFTER_THE_SCROLL_BEGAN,
              SCROLL_TOLERANCE);
}

/**
 * Given a level whose stage names the biome it already shows, once it
 * scrolled
 * When the stage goes on
 * Then the biome still began where it was first named
 */
TEST_F(LuauLevelRunFixture, KeepsWhereABiomeBeganWhenItIsNamedAgain) {
  play(NAMES_A_BIOME_AGAIN);

  tick(TICKS_JUST_SHORT_OF_A_SECOND_OF_SCROLL + 1);

  EXPECT_FLOAT_EQ(progress().biomeSince, WHERE_THE_FIRST_BIOME_BEGAN);
}

/**
 * Given a level whose stage pauses the scroll
 * When two seconds pass
 * Then the level has not scrolled, and a stage waiting for scroll still waits
 */
TEST_F(LuauLevelRunFixture, AScriptPausesTheScroll) {
  play(PAUSES_THEN_WAITS_FOR_SCROLL);

  tick(TICKS_OF_TWO_SECONDS);

  EXPECT_FLOAT_EQ(progress().scroll.position(), START_OF_THE_SCROLL);
  EXPECT_EQ(progress().biome, BIOME_BEFORE_THE_SCROLL);
}

/**
 * Given a level whose stage pauses the scroll for a wait, then resumes it
 * When the tick that resumes it passes
 * Then the level scrolled for that tick alone
 */
TEST_F(LuauLevelRunFixture, AScriptResumesTheScroll) {
  play(PAUSES_FOR_A_WAIT);

  tick(TICKS_UNTIL_THE_WAIT_IS_OVER);

  EXPECT_NEAR(progress().scroll.position(), UNITS_SCROLLED_IN_ONE_TICK,
              SCROLL_TOLERANCE);
}

/**
 * Given a level whose stage reads the scroll after a wait
 * When the wait is over
 * Then the stage read a scroll past the start
 */
TEST_F(LuauLevelRunFixture, AScriptReadsTheScroll) {
  play(READS_THE_SCROLL);

  tick(TICKS_UNTIL_THE_WAIT_IS_OVER);

  EXPECT_EQ(progress().biome, BIOME_OF_A_LEVEL_THAT_MOVED);
}

/**
 * Given a world that holds one enemy
 * When a stage counts the enemies
 * Then it counts one
 */
TEST_F(LuauLevelRunFixture, AScriptCountsTheEnemies) {
  addEnemy();
  play(COUNTS_THE_ENEMIES);

  tick(1);

  EXPECT_EQ(progress().biome, BIOME_OF_ONE_ENEMY);
}

/**
 * Given a level of two stages
 * When its first stage has not ended
 * Then the level has reached no checkpoint
 */
TEST_F(LuauLevelRunFixture, HasNoCheckpointBeforeItsFirstStageEnds) {
  play(TWO_STAGES);

  tick(TICKS_INTO_THE_FIRST_STAGE);

  EXPECT_FALSE(progress().checkpoint.has_value());
}

/**
 * Given a level of two stages
 * When its first stage ends
 * Then the checkpoint is the second stage, where the level scrolled and the
 * biome it showed then
 */
TEST_F(LuauLevelRunFixture, ReachesACheckpointWhenAStageEnds) {
  play(TWO_STAGES);

  tick(TICKS_UNTIL_THE_FIRST_STAGE_ENDS);

  const LevelCheckpoint expected{
      .stage = SECOND_STAGE,
      .travelled = SCROLL_AT_THE_CHECKPOINT,
      .biome = std::string{BIOME_OF_THE_FIRST_STAGE}};
  ASSERT_TRUE(progress().checkpoint.has_value());
  EXPECT_EQ(progress().checkpoint.value_or(LevelCheckpoint{}), expected);
}

/**
 * Given a level of two stages whose first stage ended
 * When the next tick passes
 * Then the second stage runs
 */
TEST_F(LuauLevelRunFixture, RunsTheNextStageAfterACheckpoint) {
  play(TWO_STAGES);

  tick(TICKS_UNTIL_THE_SECOND_STAGE_STARTS);

  EXPECT_EQ(progress().biome, BIOME_OF_THE_SECOND_STAGE);
}

/**
 * Given a level that has reached no checkpoint
 * When it is asked to restart from its checkpoint
 * Then it refuses, and goes on as it was
 */
TEST_F(LuauLevelRunFixture, DoesNotRestartWithoutACheckpoint) {
  play(TWO_STAGES);
  tick(TICKS_INTO_THE_FIRST_STAGE);

  const bool restarted = run().restartFromCheckpoint();

  EXPECT_FALSE(restarted);
  EXPECT_EQ(progress().biome, BIOME_OF_THE_FIRST_STAGE);
}

/**
 * Given a level deep into its second stage
 * When it restarts from its checkpoint
 * Then the scroll and the biome are those of the checkpoint
 */
TEST_F(LuauLevelRunFixture, RestartsWhereTheCheckpointWas) {
  play(TWO_STAGES);
  tick(TICKS_UNTIL_THE_SECOND_STAGE_IS_LATE);

  const bool restarted = run().restartFromCheckpoint();

  EXPECT_TRUE(restarted);
  EXPECT_EQ(progress().scroll.travelled(), SCROLL_AT_THE_CHECKPOINT);
  EXPECT_EQ(progress().biome, BIOME_OF_THE_FIRST_STAGE);
}

/**
 * Given a level deep into its second stage, whose first stage named its
 * biome 6 units in
 * When it restarts from its checkpoint
 * Then that biome began 6 units in again
 */
TEST_F(LuauLevelRunFixture, RestartsWithWhereTheBiomeOfTheCheckpointBegan) {
  play(TWO_STAGES_WITH_A_LATE_BIOME);
  tick(TICKS_UNTIL_THE_SECOND_STAGE_IS_LATE);

  static_cast<void>(run().restartFromCheckpoint());

  EXPECT_NEAR(progress().biomeSince, WHERE_THE_LATE_FIRST_BIOME_BEGAN,
              SCROLL_TOLERANCE);
}

/**
 * Given a level whose second stage paused the scroll
 * When it restarts from its checkpoint
 * Then the scroll moves again
 */
TEST_F(LuauLevelRunFixture, RestartsWithAMovingScroll) {
  play(TWO_STAGES);
  tick(TICKS_UNTIL_THE_SECOND_STAGE_IS_LATE);

  static_cast<void>(run().restartFromCheckpoint());

  EXPECT_FLOAT_EQ(progress().scroll.speed(), SPEED_OF_A_MOVING_LEVEL);
}

/**
 * Given a level that restarted from its checkpoint, its second stage having
 * gone past its wait before
 * When the next tick passes
 * Then the second stage runs again from its first line
 */
TEST_F(LuauLevelRunFixture, RunsTheStageOfTheCheckpointAgainFromItsStart) {
  play(TWO_STAGES);
  tick(TICKS_UNTIL_THE_SECOND_STAGE_IS_LATE);
  ASSERT_EQ(progress().biome, LATE_BIOME_OF_THE_SECOND_STAGE);
  static_cast<void>(run().restartFromCheckpoint());

  tick(1);

  EXPECT_EQ(progress().biome, BIOME_OF_THE_SECOND_STAGE);
}

/**
 * Given a cleared level of two stages
 * When it restarts from its checkpoint
 * Then it runs again
 */
TEST_F(LuauLevelRunFixture, AClearedLevelRunsAgainFromItsCheckpoint) {
  play(TWO_STAGES);
  tick(TICKS_UNTIL_THE_SECOND_STAGE_IS_LATE);
  ASSERT_EQ(run().state(), LuauLevelRunState::Cleared);

  static_cast<void>(run().restartFromCheckpoint());

  EXPECT_EQ(run().state(), LuauLevelRunState::Running);
}

/**
 * Given a level whose stage raises an error
 * When the stage runs
 * Then the run has failed, and says the error of the script
 */
TEST_F(LuauLevelRunFixture, FailsWhenAStageFails) {
  play(FAILS);

  tick(1);

  EXPECT_EQ(run().state(), LuauLevelRunState::Failed);
  EXPECT_THAT(run().failure(), HasSubstr(FAILURE_OF_THE_STAGE));
}

/**
 * Given a level whose stage failed after a wait
 * When more ticks pass
 * Then nothing runs any more: the level keeps the biome it showed
 */
TEST_F(LuauLevelRunFixture, RunsNothingOnceFailed) {
  play(FAILS_AFTER_A_WAIT);
  tick(TICKS_UNTIL_THE_WAIT_IS_OVER);
  ASSERT_EQ(run().state(), LuauLevelRunState::Failed);

  tick(TICKS_UNTIL_THE_WAIT_IS_OVER);

  EXPECT_EQ(progress().biome, BIOME_BEFORE_THE_WAIT);
  EXPECT_FALSE(run().restartFromCheckpoint());
}

/**
 * Given a level whose stage yields without a wait of level
 * When the stage runs
 * Then the run has failed, saying a stage only yields through its waits
 */
TEST_F(LuauLevelRunFixture, FailsWhenAStageYieldsByItself) {
  play(YIELDS_BY_ITSELF);

  tick(1);

  EXPECT_EQ(run().state(), LuauLevelRunState::Failed);
  EXPECT_THAT(run().failure(), HasSubstr(rtype::game::LEVEL_YIELD_MESSAGE));
}

/**
 * Given a level whose stage names a biome in capitals and spaces
 * When the stage runs
 * Then the run has failed, saying what a biome name is, and no biome shows
 */
TEST_F(LuauLevelRunFixture, RefusesABiomeNameTheProtocolCannotCarry) {
  play(NAMES_A_WRONG_BIOME);

  tick(1);

  EXPECT_THAT(run().failure(),
              HasSubstr(rtype::game::WRONG_BIOME_NAME_MESSAGE));
  EXPECT_TRUE(progress().biome.empty());
}
