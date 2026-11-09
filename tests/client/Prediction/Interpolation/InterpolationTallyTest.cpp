#include <gtest/gtest.h>
#include <optional>
#include "InterpolationTally.hpp"
#include "InterpolationTallyTestConstants.hpp"

namespace rtype::client {

using testing::DELAY_BEFORE_THE_NEWEST;
using testing::DELAY_TOLERANCE;
using testing::DRAWN_BEFORE_THE_NEWEST;
using testing::DRAWN_IN_THE_NEXT_GAME;
using testing::DRAWN_PAST_THE_NEWEST;
using testing::FIRST_TICK_OF_THE_NEXT_GAME;
using testing::NEWEST_STATE_TICK;
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

}  // namespace rtype::client
