#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include "BossStageTestConstants.hpp"
#include "LevelBossesTestConstants.hpp"
#include "LevelDirector.hpp"
#include "LevelDirectorFixture.hpp"
#include "LevelDirectorTestConstants.hpp"
#include "ServerException.hpp"

using rtype::server::stagesOfLevel;
using rtype::server::UnknownLevelException;
using rtype::server::testing::CAVES_BIOME;
using rtype::server::testing::LEVEL_ENEMY_SHEET;
using rtype::server::testing::LEVEL_OF_ONE_BIOME;
using rtype::server::testing::LEVEL_OF_TWO_BIOMES;
using rtype::server::testing::LEVEL_OF_TWO_STAGES;
using rtype::server::testing::LEVEL_SHIP_SHEET;
using rtype::server::testing::LEVEL_THAT_FAILS;
using rtype::server::testing::LEVEL_THAT_PAUSES;
using rtype::server::testing::LEVEL_THAT_WAITS;
using rtype::server::testing::LEVEL_WITH_A_BOSS;
using rtype::server::testing::LEVEL_WITH_A_BOSS_AFTER_A_CHECKPOINT;
using rtype::server::testing::LevelDirectorFixture;
using rtype::server::testing::NO_BOSS;
using rtype::server::testing::ONE_BOSS;
using rtype::server::testing::SCROLL_OF_ONE_SECOND;
using rtype::server::testing::SCROLL_OF_ONE_TICK;
using rtype::server::testing::SCROLL_ONE_TICK_AFTER_THE_CHECKPOINT;
using rtype::server::testing::SCROLL_TOLERANCE;
using rtype::server::testing::SPEED_OF_A_SCROLLING_LEVEL;
using rtype::server::testing::SPEED_OF_A_STOPPED_LEVEL;
using rtype::server::testing::TICK_OF_THE_SECOND_ACT;
using rtype::server::testing::TICKS_INTO_THE_SECOND_STAGE;
using rtype::server::testing::TICKS_OF_ONE_SECOND;
using rtype::server::testing::TICKS_TO_THE_BOSS;
using rtype::server::testing::TICKS_UNTIL_THE_BOSS_ENTERS;
using rtype::server::testing::TICKS_UNTIL_THE_SECOND_STAGE_ANNOUNCES;
using rtype::server::testing::UNREGISTERED_LEVEL;
using rtype::server::testing::WHERE_THE_ENEMY_IS;
using rtype::server::testing::WHERE_THE_SHIP_IS;
using rtype::server::testing::WRECK_BIOME;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::SizeIs;

/**
 * Given a level whose stage names its biome
 * When the first tick passes
 * Then the game is told that biome, a level that scrolls, one tick in
 */
TEST_F(LevelDirectorFixture, TellsTheLevelAtItsFirstTick) {
  direct(LEVEL_OF_ONE_BIOME);

  tick(1);

  ASSERT_THAT(changes(), SizeIs(1));
  EXPECT_EQ(changes().front().biome, WRECK_BIOME);
  EXPECT_FLOAT_EQ(changes().front().scrollSpeed, SPEED_OF_A_SCROLLING_LEVEL);
  EXPECT_NEAR(changes().front().scrollPosition, SCROLL_OF_ONE_TICK,
              SCROLL_TOLERANCE);
}

/**
 * Given a level told at its first tick
 * When a second passes without the level changing
 * Then the game is told nothing more
 */
TEST_F(LevelDirectorFixture, TellsNothingWhileTheLevelDoesNotChange) {
  direct(LEVEL_THAT_WAITS);
  tick(1);

  tick(TICKS_OF_ONE_SECOND);

  EXPECT_THAT(changes(), SizeIs(1));
}

/**
 * Given a level whose stage pauses the scroll at its fourth tick
 * When that tick passes
 * Then the game is told a level that no longer scrolls
 */
TEST_F(LevelDirectorFixture, TellsTheScrollStopped) {
  direct(LEVEL_THAT_PAUSES);

  tick(TICK_OF_THE_SECOND_ACT);

  ASSERT_THAT(changes(), SizeIs(2));
  EXPECT_FLOAT_EQ(changes().back().scrollSpeed, SPEED_OF_A_STOPPED_LEVEL);
}

/**
 * Given a level whose stage names another biome at its fourth tick
 * When that tick passes
 * Then the game is told the new biome
 */
TEST_F(LevelDirectorFixture, TellsTheBiomeChanged) {
  direct(LEVEL_OF_TWO_BIOMES);

  tick(TICK_OF_THE_SECOND_ACT);

  ASSERT_THAT(changes(), SizeIs(2));
  EXPECT_EQ(changes().back().biome, CAVES_BIOME);
}

/**
 * Given a level nothing pauses
 * When one second of ticks passes
 * Then the level scrolled 120 logical units, which anyone may read
 */
TEST_F(LevelDirectorFixture, ScrollsTheLevelEachTick) {
  direct(LEVEL_THAT_WAITS);

  tick(TICKS_OF_ONE_SECOND);

  EXPECT_NEAR(director().progress().scroll.position(), SCROLL_OF_ONE_SECOND,
              SCROLL_TOLERANCE);
}

/**
 * Given a level whose last stage returned, in a world without any enemy
 * When the director is asked
 * Then the level is cleared
 */
TEST_F(LevelDirectorFixture, IsClearedOnceItsLastStageReturned) {
  direct(LEVEL_OF_ONE_BIOME);

  tick(1);

  EXPECT_TRUE(director().cleared());
}

/**
 * Given a level whose last stage returned, an enemy still in the world
 * When the director is asked
 * Then the level is not cleared
 */
TEST_F(LevelDirectorFixture, IsNotClearedWhileAnEnemyIsLeft) {
  direct(LEVEL_OF_ONE_BIOME);
  place(LEVEL_ENEMY_SHEET, WHERE_THE_ENEMY_IS);

  tick(1);

  EXPECT_FALSE(director().cleared());
}

/**
 * Given a level whose stage still waits, in a world without any enemy
 * When the director is asked
 * Then the level is not cleared
 */
TEST_F(LevelDirectorFixture, IsNotClearedWhileAStageRuns) {
  direct(LEVEL_THAT_WAITS);

  tick(1);

  EXPECT_FALSE(director().cleared());
}

/**
 * Given a level whose stage raises an error
 * When the stage runs
 * Then the level has failed
 */
TEST_F(LevelDirectorFixture, HasFailedOnceAStageFailed) {
  direct(LEVEL_THAT_FAILS);

  tick(1);

  EXPECT_TRUE(director().failed());
}

/**
 * Given a level still in its first stage, an enemy in the world
 * When it is asked to restart from its checkpoint
 * Then it refuses and removes nothing
 */
TEST_F(LevelDirectorFixture, DoesNotRestartBeforeItsFirstCheckpoint) {
  direct(LEVEL_OF_TWO_STAGES);
  place(LEVEL_ENEMY_SHEET, WHERE_THE_ENEMY_IS);
  tick(1);

  const bool restarted = director().restartFromCheckpoint();
  tick(1);

  EXPECT_FALSE(restarted);
  EXPECT_THAT(removed(), IsEmpty());
}

/**
 * Given a level past its checkpoint, an enemy and a ship in the world
 * When it restarts from its checkpoint
 * Then the enemy is removed and the ship stays
 */
TEST_F(LevelDirectorFixture, RemovesEveryBodyButTheShipsOnARestart) {
  direct(LEVEL_OF_TWO_STAGES);
  const std::uint32_t enemy = place(LEVEL_ENEMY_SHEET, WHERE_THE_ENEMY_IS);
  place(LEVEL_SHIP_SHEET, WHERE_THE_SHIP_IS);
  tick(TICKS_INTO_THE_SECOND_STAGE);

  const bool restarted = director().restartFromCheckpoint();
  tick(1);

  EXPECT_TRUE(restarted);
  EXPECT_THAT(removed(), ElementsAre(enemy));
}

/**
 * Given a level past its checkpoint, reached after 6 units of scroll
 * When it restarts from its checkpoint and a tick passes
 * Then the game is told the level again, one tick after the checkpoint
 */
TEST_F(LevelDirectorFixture, TellsTheLevelAgainAfterARestart) {
  direct(LEVEL_OF_TWO_STAGES);
  tick(TICKS_INTO_THE_SECOND_STAGE);
  const std::size_t toldBefore = changes().size();
  static_cast<void>(director().restartFromCheckpoint());

  tick(1);

  ASSERT_THAT(changes(), SizeIs(toldBefore + 1));
  EXPECT_NEAR(changes().back().scrollPosition,
              SCROLL_ONE_TICK_AFTER_THE_CHECKPOINT, SCROLL_TOLERANCE);
}

/**
 * Given scripts that register one level
 * When the stages of another level are asked for
 * Then the level is refused as unknown
 */
TEST_F(LevelDirectorFixture, RefusesALevelNoScriptRegistered) {
  direct(LEVEL_OF_ONE_BIOME);

  EXPECT_THROW(static_cast<void>(stagesOfLevel(
                   scripts().levels(), std::string{UNREGISTERED_LEVEL})),
               UnknownLevelException);
}

/**
 * Given a level whose stage brings the Bydo Tyrant in
 * When the warning of its arrival is over
 * Then the boss is in the world
 */
TEST_F(LevelDirectorFixture, BringsTheBossAStageNamesIn) {
  direct(LEVEL_WITH_A_BOSS);

  tick(TICKS_TO_THE_BOSS);

  EXPECT_EQ(bossCount(), ONE_BOSS);
}

/**
 * Given a level whose second stage announced the Bydo Tyrant
 * When the level restarts from its checkpoint and that warning would be over
 * Then the boss announced before the restart never entered
 */
TEST_F(LevelDirectorFixture, ForgetsTheBossAnnouncedBeforeARestart) {
  direct(LEVEL_WITH_A_BOSS_AFTER_A_CHECKPOINT);
  tick(TICKS_UNTIL_THE_SECOND_STAGE_ANNOUNCES);

  ASSERT_TRUE(director().restartFromCheckpoint());
  tick(TICKS_UNTIL_THE_BOSS_ENTERS);

  EXPECT_EQ(bossCount(), NO_BOSS);
}
