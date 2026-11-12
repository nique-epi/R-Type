#include <gtest/gtest.h>
#include <optional>
#include "ClockRecalibrations.hpp"
#include "InterpolationTally.hpp"
#include "InterpolationTallyTestConstants.hpp"

namespace rtype::client {

using testing::BOTH_MOVES_TICKS;
using testing::DELAY_BEFORE_THE_NEWEST;
using testing::DELAY_TOLERANCE;
using testing::DRAWN_BEFORE_THE_NEWEST;
using testing::DRAWN_IN_THE_NEXT_GAME;
using testing::DRAWN_PAST_THE_NEWEST;
using testing::FIRST_MOVE_TICKS;
using testing::FIRST_TICK_OF_THE_NEXT_GAME;
using testing::NEWEST_STATE_TICK;
using testing::SECOND_MOVE_TICKS;
using testing::SERVER_TICK_NOW;

/**
 * Given a tally of a game that received no state yet
 * When a frame draws tick 300.33
 * Then nothing is counted: the game shows no entity of the server
 */
TEST(InterpolationTally, CountsNoFrameBeforeTheFirstState) {
  InterpolationTally tally;
  tally.startGame();

  tally.countFrame(DRAWN_PAST_THE_NEWEST, SERVER_TICK_NOW);

  EXPECT_EQ(tally.pastTheNewest(), 0U);
  EXPECT_FALSE(tally.delayTicks().has_value());
}

/**
 * Given a tally whose newest state is tick 300
 * When a frame draws tick 299.66, then a frame draws tick 300.33
 * Then one frame is counted past the newest state: the second
 */
TEST(InterpolationTally, CountsTheFramesPastTheNewestState) {
  InterpolationTally tally;
  tally.observe(NEWEST_STATE_TICK);

  tally.countFrame(DRAWN_BEFORE_THE_NEWEST, SERVER_TICK_NOW);
  tally.countFrame(DRAWN_PAST_THE_NEWEST, SERVER_TICK_NOW);

  EXPECT_EQ(tally.pastTheNewest(), 1U);
}

/**
 * Given a tally whose newest state is tick 300
 * When a frame draws tick 299.66 while the server is at tick 308
 * Then the newest frame drew 8.34 ticks behind the server
 */
TEST(InterpolationTally, KeepsHowFarBehindTheServerTheNewestFrameDrew) {
  InterpolationTally tally;
  tally.observe(NEWEST_STATE_TICK);

  tally.countFrame(DRAWN_BEFORE_THE_NEWEST, SERVER_TICK_NOW);

  EXPECT_NEAR(tally.delayTicks().value_or(0.0), DELAY_BEFORE_THE_NEWEST,
              DELAY_TOLERANCE);
}

/**
 * Given a tally whose newest state of a game was tick 300
 * When the next game starts and a frame draws tick 3.5 past its first state,
 * tick 3
 * Then that frame counts as past the newest state of the new game, and the
 * totals of the first game stay
 */
TEST(InterpolationTally, StartsTheTicksOverWithEachGame) {
  InterpolationTally tally;
  tally.observe(NEWEST_STATE_TICK);
  tally.countFrame(DRAWN_BEFORE_THE_NEWEST, SERVER_TICK_NOW);
  tally.startGame();
  tally.observe(FIRST_TICK_OF_THE_NEXT_GAME);

  tally.countFrame(DRAWN_IN_THE_NEXT_GAME, std::nullopt);

  EXPECT_EQ(tally.pastTheNewest(), 1U);
}

/**
 * Given a tally whose newest frame drew 8.34 ticks behind the server
 * When the game ends
 * Then no delay is kept: no game draws any more
 */
TEST(InterpolationTally, KeepsNoDelayOnceTheGameEnded) {
  InterpolationTally tally;
  tally.observe(NEWEST_STATE_TICK);
  tally.countFrame(DRAWN_BEFORE_THE_NEWEST, SERVER_TICK_NOW);

  tally.endGame();

  EXPECT_FALSE(tally.delayTicks().has_value());
}

/**
 * Given a game that ended
 * When a late state of tick 300 arrives, then a frame draws tick 300.33
 * Then nothing is counted until the next game starts
 */
TEST(InterpolationTally, CountsNoFrameOnceTheGameEnded) {
  InterpolationTally tally;
  tally.endGame();

  tally.observe(NEWEST_STATE_TICK);
  tally.countFrame(DRAWN_PAST_THE_NEWEST, SERVER_TICK_NOW);

  EXPECT_EQ(tally.pastTheNewest(), 0U);
  EXPECT_FALSE(tally.delayTicks().has_value());
}

/**
 * Given a game that ended
 * When the next game starts, receives tick 3 and a frame draws tick 3.5
 * Then that frame is counted again
 */
TEST(InterpolationTally, CountsAgainOnceTheNextGameStarts) {
  InterpolationTally tally;
  tally.endGame();
  tally.startGame();
  tally.observe(FIRST_TICK_OF_THE_NEXT_GAME);

  tally.countFrame(DRAWN_IN_THE_NEXT_GAME, std::nullopt);

  EXPECT_EQ(tally.pastTheNewest(), 1U);
}

/**
 * Given a tally of a game
 * When states move the tick shown by 0.25, then by 0.5 ticks
 * Then two moves are counted, 0.75 ticks in all
 */
TEST(InterpolationTally, AddsUpTheMovesOfTheTickShown) {
  InterpolationTally tally;

  tally.countRecalibration(FIRST_MOVE_TICKS);
  tally.countRecalibration(SECOND_MOVE_TICKS);

  EXPECT_EQ(tally.recalibrations(),
            (ClockRecalibrations{.count = 2, .ticks = BOTH_MOVES_TICKS}));
}

/**
 * Given a game that ended
 * When a late state moves the tick shown
 * Then no move is counted
 */
TEST(InterpolationTally, CountsNoMoveOnceTheGameEnded) {
  InterpolationTally tally;
  tally.endGame();

  tally.countRecalibration(FIRST_MOVE_TICKS);

  EXPECT_EQ(tally.recalibrations(), ClockRecalibrations{});
}

}  // namespace rtype::client
