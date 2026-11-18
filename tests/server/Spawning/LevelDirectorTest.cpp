#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include "LdtkPlacementFixtures.hpp"
#include "LdtkPlacementTestConstants.hpp"
#include "LevelCheckpoint.hpp"
#include "LevelDirector.hpp"
#include "LevelDirectorFixture.hpp"
#include "LevelDirectorTestConstants.hpp"
#include "PlayBand.hpp"
#include "ServerException.hpp"

using rtype::server::entryOfLevel;
using rtype::server::startOfStage;
using rtype::server::UnknownLevelException;
using rtype::server::UnknownLevelStartException;
using rtype::server::testing::CAVES_BAND;
using rtype::server::testing::CAVES_BIOME;
using rtype::server::testing::DIRECTED_LEVEL;
using rtype::server::testing::FIRST_STAGE;
using rtype::server::testing::LEVEL_ENEMY_SHEET;
using rtype::server::testing::LEVEL_OF_ONE_BIOME;
using rtype::server::testing::LEVEL_OF_TWO_BIOMES;
using rtype::server::testing::LEVEL_OF_TWO_BIOMES_THE_SECOND_CLOSED;
using rtype::server::testing::LEVEL_OF_TWO_STAGES;
using rtype::server::testing::LEVEL_OF_TWO_STAGES_THAT_SPAWNS_THE_PLACED;
using rtype::server::testing::LEVEL_SHIP_SHEET;
using rtype::server::testing::LEVEL_THAT_FAILS;
using rtype::server::testing::LEVEL_THAT_PAUSES;
using rtype::server::testing::LEVEL_THAT_PLAYS_A_TRANSITION;
using rtype::server::testing::LEVEL_THAT_SPAWNS_THE_PLACED;
using rtype::server::testing::LEVEL_THAT_WAITS;
using rtype::server::testing::LEVEL_WITH_A_DECLARED_START;
using rtype::server::testing::LevelDirectorFixture;
using rtype::server::testing::SCROLL_OF_ONE_SECOND;
using rtype::server::testing::SCROLL_OF_ONE_TICK;
using rtype::server::testing::SCROLL_OF_THE_DECLARED_START;
using rtype::server::testing::SCROLL_ONE_TICK_AFTER_THE_CHECKPOINT;
using rtype::server::testing::SCROLL_TOLERANCE;
using rtype::server::testing::SCROLLED_ONE_SECOND_X;
using rtype::server::testing::SCROLLED_UNTIL_THE_PAUSE_X;
using rtype::server::testing::SECOND_STAGE;
using rtype::server::testing::SECOND_STAGE_FROM_ZERO;
using rtype::server::testing::SPAWNS_AFTER_THE_RESTART;
using rtype::server::testing::SPAWNS_BEFORE_THE_RESTART;
using rtype::server::testing::SPEED_OF_A_SCROLLING_LEVEL;
using rtype::server::testing::SPEED_OF_A_STOPPED_LEVEL;
using rtype::server::testing::TICK_OF_THE_SECOND_ACT;
using rtype::server::testing::TICKS_BEFORE_THE_DRIFTER_COMES_IN;
using rtype::server::testing::TICKS_INTO_THE_SECOND_STAGE;
using rtype::server::testing::TICKS_OF_ONE_SECOND;
using rtype::server::testing::TICKS_PAST_THE_PAUSE;
using rtype::server::testing::TICKS_UNTIL_BOTH_CAME_IN;
using rtype::server::testing::TICKS_UNTIL_THE_LATE_ONE_COMES_IN_AGAIN;
using rtype::server::testing::TICKS_UNTIL_THE_TRANSITION_ENDS;
using rtype::server::testing::TRANSITION_LEFT_AS_IT_STARTS;
using rtype::server::testing::TRAVELLED_TO_THE_DECLARED_START;
using rtype::server::testing::UNREGISTERED_LEVEL;
using rtype::server::testing::WHERE_THE_DRIFTER_APPEARS_X;
using rtype::server::testing::WHERE_THE_DRIFTER_IS_PLACED;
using rtype::server::testing::WHERE_THE_EARLY_DRIFTER_IS;
using rtype::server::testing::WHERE_THE_ENEMY_IS;
using rtype::server::testing::WHERE_THE_LATE_DRIFTER_IS;
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
 * Given a level whose first biome declares no band
 * When its first tick passes
 * Then the game is told the ships fly in the whole play area
 */
TEST_F(LevelDirectorFixture, TellsTheWholePlayAreaForABiomeWithoutBand) {
  direct(LEVEL_OF_TWO_BIOMES_THE_SECOND_CLOSED);

  tick(1);

  ASSERT_THAT(changes(), SizeIs(1));
  EXPECT_EQ(changes().back().band, rtype::game::PlayBand{});
}

/**
 * Given a level whose second biome, caves, is closed from row 200 to row 800
 * When the stage names that biome
 * Then the game is told the band of caves with it
 */
TEST_F(LevelDirectorFixture, TellsTheBandOfTheBiome) {
  direct(LEVEL_OF_TWO_BIOMES_THE_SECOND_CLOSED);

  tick(TICK_OF_THE_SECOND_ACT);

  ASSERT_THAT(changes(), SizeIs(2));
  EXPECT_EQ(changes().back().band, CAVES_BAND);
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

  EXPECT_THROW(static_cast<void>(entryOfLevel(scripts().levels(),
                                              std::string{UNREGISTERED_LEVEL})),
               UnknownLevelException);
}

/**
 * Given a level nothing pauses and a Bydo made to follow the scroll at x 900
 * When one second of ticks passes
 * Then the Bydo is 120 units further left, as far as the level scrolled
 */
TEST_F(LevelDirectorFixture, ABodyFollowingTheScrollMovesAsFarAsTheLevel) {
  direct(LEVEL_THAT_WAITS);
  const std::uint32_t wall =
      placeFollowingTheScroll(LEVEL_ENEMY_SHEET, WHERE_THE_ENEMY_IS);

  tick(TICKS_OF_ONE_SECOND);

  EXPECT_NEAR(positionOf(wall).x, SCROLLED_ONE_SECOND_X, SCROLL_TOLERANCE);
  EXPECT_FLOAT_EQ(positionOf(wall).y, WHERE_THE_ENEMY_IS.y);
}

/**
 * Given a level that pauses its scroll at its fourth tick, and a Bydo made to
 * follow the scroll at x 900
 * When five ticks pass
 * Then the Bydo moved only through the three ticks before the pause
 */
TEST_F(LevelDirectorFixture, ABodyFollowingTheScrollStopsWithIt) {
  direct(LEVEL_THAT_PAUSES);
  const std::uint32_t wall =
      placeFollowingTheScroll(LEVEL_ENEMY_SHEET, WHERE_THE_ENEMY_IS);

  tick(TICKS_PAST_THE_PAUSE);

  EXPECT_NEAR(positionOf(wall).x, SCROLLED_UNTIL_THE_PAUSE_X, SCROLL_TOLERANCE);
}

/**
 * Given a level nothing pauses and a Bydo at x 900 that does not follow the
 * scroll
 * When one second of ticks passes
 * Then the director has not moved it
 */
TEST_F(LevelDirectorFixture, ABodyThatDoesNotFollowTheScrollStays) {
  direct(LEVEL_THAT_WAITS);
  const std::uint32_t bydo = place(LEVEL_ENEMY_SHEET, WHERE_THE_ENEMY_IS);

  tick(TICKS_OF_ONE_SECOND);

  EXPECT_FLOAT_EQ(positionOf(bydo).x, WHERE_THE_ENEMY_IS.x);
}

/**
 * Given a level that places a Drifter 2224 units in, whose stage asks for the
 * placed enemies
 * When the ticks pass until the level scrolled just short of 256 units, 128
 * ticks of scroll
 * Then no enemy was asked for yet
 */
TEST_F(LevelDirectorFixture, HoldsAPlacedEnemyUntilTheScrollBringsItIn) {
  placeTheDrifter();
  direct(LEVEL_THAT_SPAWNS_THE_PLACED);

  tick(TICKS_BEFORE_THE_DRIFTER_COMES_IN - 1);

  EXPECT_THAT(spawned(), IsEmpty());
}

/**
 * Given the same level, ticked until the level scrolled past 256 units, 129
 * ticks of scroll
 * When the next tick passes
 * Then the Drifter is asked for, its left edge on the right edge of the
 * playfield, at x 1966
 */
TEST_F(LevelDirectorFixture, SpawnsAPlacedEnemyWhenTheScrollBringsItIn) {
  placeTheDrifter();
  direct(LEVEL_THAT_SPAWNS_THE_PLACED);
  tick(TICKS_BEFORE_THE_DRIFTER_COMES_IN);

  tick(1);

  ASSERT_THAT(spawned(), SizeIs(1));
  EXPECT_EQ(spawned().front().type, PLACED_DRIFTER_SHEET);
  EXPECT_NEAR(spawned().front().position.x, WHERE_THE_DRIFTER_APPEARS_X,
              SCROLL_TOLERANCE);
  EXPECT_FLOAT_EQ(spawned().front().position.y, WHERE_THE_DRIFTER_IS_PLACED.y);
}

/**
 * Given a level that places a Drifter, whose stage never asks for the placed
 * enemies
 * When the level scrolls past it
 * Then no enemy is asked for
 */
TEST_F(LevelDirectorFixture, SpawnsNothingWhileNoStageAskedForThePlaced) {
  placeTheDrifter();
  direct(LEVEL_THAT_WAITS);

  tick(TICKS_BEFORE_THE_DRIFTER_COMES_IN + 1);

  EXPECT_THAT(spawned(), IsEmpty());
}

/**
 * Given a level of two stages whose checkpoint is at 6 units of scroll, with
 * a Drifter coming in at 4 units and another at 20, both come in
 * When the level restarts from its checkpoint and scrolls past both again
 * Then only the late Drifter is asked for again
 */
TEST_F(LevelDirectorFixture, OnlyTheEnemiesPlacedAfterTheCheckpointComeBack) {
  placeEnemies({placedEnemyOf(PLACED_DRIFTER_SHEET, WHERE_THE_EARLY_DRIFTER_IS,
                              PLACED_ENEMY_WIDTH),
                placedEnemyOf(PLACED_DRIFTER_SHEET, WHERE_THE_LATE_DRIFTER_IS,
                              PLACED_ENEMY_WIDTH)});
  direct(LEVEL_OF_TWO_STAGES_THAT_SPAWNS_THE_PLACED);
  tick(TICKS_UNTIL_BOTH_CAME_IN);
  ASSERT_THAT(spawned(), SizeIs(SPAWNS_BEFORE_THE_RESTART));

  ASSERT_TRUE(director().restartFromCheckpoint());
  tick(TICKS_UNTIL_THE_LATE_ONE_COMES_IN_AGAIN);

  ASSERT_THAT(spawned(), SizeIs(SPAWNS_AFTER_THE_RESTART));
  EXPECT_FLOAT_EQ(spawned().back().position.y, WHERE_THE_LATE_DRIFTER_IS.y);
}

/**
 * Given a level whose stage plays a transition of 3 s at its first tick
 * When that tick passes
 * Then the game is told the time left of the transition, 2983 ms
 */
TEST_F(LevelDirectorFixture, TellsTheTimeLeftOfATransitionAsItStarts) {
  direct(LEVEL_THAT_PLAYS_A_TRANSITION);

  tick(1);

  ASSERT_THAT(changes(), SizeIs(1));
  EXPECT_EQ(changes().back().transitionLeft, TRANSITION_LEFT_AS_IT_STARTS);
}

/**
 * Given a level whose stage plays a transition of 3 s
 * When its 180 ticks pass
 * Then the game is told the scroll moves again, with no transition left
 */
TEST_F(LevelDirectorFixture, LetsTheScrollMoveOnOnceTheTransitionEnded) {
  direct(LEVEL_THAT_PLAYS_A_TRANSITION);

  tick(TICKS_UNTIL_THE_TRANSITION_ENDS);

  ASSERT_THAT(changes(), SizeIs(2));
  EXPECT_GT(changes().back().scrollSpeed, 0.0F);
  EXPECT_EQ(changes().back().transitionLeft, 0);
}

/**
 * Given a level whose script declares no start, and a game set to its first
 * stage
 * When the start of that stage is asked for
 * Then there is none: the game starts at the start of the level
 */
TEST_F(LevelDirectorFixture, AGameAtTheFirstStageStartsAtTheStartOfTheLevel) {
  direct(LEVEL_OF_TWO_STAGES);

  EXPECT_FALSE(startOfStage(*scripts().levels().entryOf(DIRECTED_LEVEL),
                            std::string{DIRECTED_LEVEL}, FIRST_STAGE)
                   .has_value());
}

/**
 * Given a level whose second stage starts at 600 units of scroll in the caves
 * When the start of its second stage is asked for
 * Then it is that stage, counted from 0, in the caves begun at 600 units
 */
TEST_F(LevelDirectorFixture, AGameAtADeclaredStageStartsWhereItIsDeclared) {
  direct(LEVEL_WITH_A_DECLARED_START);

  const auto start = startOfStage(*scripts().levels().entryOf(DIRECTED_LEVEL),
                                  std::string{DIRECTED_LEVEL}, SECOND_STAGE);

  ASSERT_TRUE(start.has_value());
  const rtype::game::LevelCheckpoint found =
      start.value_or(rtype::game::LevelCheckpoint{});
  EXPECT_EQ(found.stage, SECOND_STAGE_FROM_ZERO);
  EXPECT_EQ(found.biome, CAVES_BIOME);
  EXPECT_FLOAT_EQ(found.biomeSince, SCROLL_OF_THE_DECLARED_START);
}

/**
 * Given a level whose second stage starts at 600 units of scroll
 * When the start of its second stage is asked for
 * Then the level has scrolled the 5 s of those 600 units at 120 per second
 */
TEST_F(LevelDirectorFixture, ADeclaredStartScrolledTheTimeOfItsScroll) {
  direct(LEVEL_WITH_A_DECLARED_START);

  const auto start = startOfStage(*scripts().levels().entryOf(DIRECTED_LEVEL),
                                  std::string{DIRECTED_LEVEL}, SECOND_STAGE);

  EXPECT_EQ(start.value_or(rtype::game::LevelCheckpoint{}).travelled,
            TRAVELLED_TO_THE_DECLARED_START);
}

/**
 * Given a level of two stages whose script declares no start
 * When the start of its second stage is asked for
 * Then it is refused
 */
TEST_F(LevelDirectorFixture, RefusesAStageWithoutADeclaredStart) {
  direct(LEVEL_OF_TWO_STAGES);

  EXPECT_THROW(static_cast<void>(
                   startOfStage(*scripts().levels().entryOf(DIRECTED_LEVEL),
                                std::string{DIRECTED_LEVEL}, SECOND_STAGE)),
               UnknownLevelStartException);
}
