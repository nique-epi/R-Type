#include <gtest/gtest.h>
#include "BotReport.hpp"
#include "BotTally.hpp"
#include "BotTallyFixtures.hpp"
#include "BotTallyTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Duration;
using rtype::tools::BotTally;
using rtype::tools::testing::A_FRAME;
using rtype::tools::testing::FIRST_TICK;
using rtype::tools::testing::gameEnded;
using rtype::tools::testing::SECOND_TICK;
using rtype::tools::testing::THE_GAME_RUNS;
using rtype::tools::testing::THE_SESSION_ENDED_IT;
using rtype::tools::testing::THIRD_TICK;
using rtype::tools::testing::THREE_WORLD_STATES;
using rtype::tools::testing::TWO_FRAMES;
using rtype::tools::testing::TWO_WORLD_STATES;
using rtype::tools::testing::worldStateAt;

/**
 * Given a tally
 * When it counts two parts of the world state of one tick
 * Then it counts one world state
 */
TEST(BotTally, TwoPartsOfOneTickCountOneWorldState) {
  BotTally tally;

  tally.count(worldStateAt(FIRST_TICK));
  tally.count(worldStateAt(FIRST_TICK));

  EXPECT_EQ(tally.report().worldStates, 1U);
}

/**
 * Given a tally
 * When it counts the world states of three ticks in a row
 * Then it counts three world states
 */
TEST(BotTally, EachNewTickCountsOneWorldState) {
  BotTally tally;

  tally.count(worldStateAt(FIRST_TICK));
  tally.count(worldStateAt(SECOND_TICK));
  tally.count(worldStateAt(THIRD_TICK));

  EXPECT_EQ(tally.report().worldStates, THREE_WORLD_STATES);
}

/**
 * Given a tally that counted a world state
 * When it counts the end of the game
 * Then it counts one game ended
 */
TEST(BotTally, TheEndOfAGameCountsOneGame) {
  BotTally tally;
  tally.count(worldStateAt(FIRST_TICK));

  tally.count(gameEnded());

  EXPECT_EQ(tally.report().gamesEnded, 1U);
}

/**
 * Given a tally that counted a world state, then the end of its game
 * When it counts a world state of the same tick, from the next game
 * Then it counts that world state too
 */
TEST(BotTally, TheNextGameCountsItsTicksAgain) {
  BotTally tally;
  tally.count(worldStateAt(FIRST_TICK));
  tally.count(gameEnded());

  tally.count(worldStateAt(FIRST_TICK));

  EXPECT_EQ(tally.report().worldStates, TWO_WORLD_STATES);
}

/**
 * Given a tally that counted no world state
 * When a frame passes while the session says the game runs
 * Then no time in game is counted
 */
TEST(BotTally, NoTimeCountsBeforeTheFirstWorldState) {
  BotTally tally;

  tally.addTime(A_FRAME, THE_GAME_RUNS);

  EXPECT_EQ(tally.report().timeInGame, Duration::zero());
}

/**
 * Given a tally that counted a world state
 * When two frames pass while the game runs
 * Then both frames are counted in game
 */
TEST(BotTally, FramesCountFromTheFirstWorldState) {
  BotTally tally;
  tally.count(worldStateAt(FIRST_TICK));

  tally.addTime(A_FRAME, THE_GAME_RUNS);
  tally.addTime(A_FRAME, THE_GAME_RUNS);

  EXPECT_EQ(tally.report().timeInGame, TWO_FRAMES);
}

/**
 * Given a tally that counted a world state
 * When a frame passes after the session ended the game
 * Then no time in game is counted
 */
TEST(BotTally, NoTimeCountsOnceTheSessionEndedTheGame) {
  BotTally tally;
  tally.count(worldStateAt(FIRST_TICK));

  tally.addTime(A_FRAME, THE_SESSION_ENDED_IT);

  EXPECT_EQ(tally.report().timeInGame, Duration::zero());
}

/**
 * Given a tally that counted a world state, then the end of its game
 * When a frame passes, the session still saying the game runs
 * Then no time in game is counted
 */
TEST(BotTally, NoTimeCountsAfterTheEndOfAGame) {
  BotTally tally;
  tally.count(worldStateAt(FIRST_TICK));
  tally.count(gameEnded());

  tally.addTime(A_FRAME, THE_GAME_RUNS);

  EXPECT_EQ(tally.report().timeInGame, Duration::zero());
}

/**
 * Given a tally that counted no world state
 * When the bot sends an input
 * Then no input in game is counted
 */
TEST(BotTally, NoInputCountsBeforeTheFirstWorldState) {
  BotTally tally;

  tally.countInput();

  EXPECT_EQ(tally.report().inputsInGame, 0U);
}

/**
 * Given a tally that counted a world state
 * When the bot sends an input
 * Then one input in game is counted
 */
TEST(BotTally, AnInputCountsDuringAGame) {
  BotTally tally;
  tally.count(worldStateAt(FIRST_TICK));

  tally.countInput();

  EXPECT_EQ(tally.report().inputsInGame, 1U);
}

/**
 * Given a tally that counted a world state, then the end of its game
 * When the bot sends an input
 * Then no input in game is counted
 */
TEST(BotTally, NoInputCountsAfterTheEndOfAGame) {
  BotTally tally;
  tally.count(worldStateAt(FIRST_TICK));
  tally.count(gameEnded());

  tally.countInput();

  EXPECT_EQ(tally.report().inputsInGame, 0U);
}
