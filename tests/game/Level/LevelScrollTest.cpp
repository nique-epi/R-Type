#include <gtest/gtest.h>
#include "LevelScroll.hpp"
#include "LevelScrollTestConstants.hpp"

using rtype::game::LevelScroll;

/**
 * Given a level that has not scrolled yet
 * When it scrolls for one second
 * Then it is 120 logical units further
 */
TEST(LevelScroll, ScrollsAtItsSpeed) {
  LevelScroll scroll;

  scroll.advance(ONE_SECOND_OF_SCROLL);

  EXPECT_FLOAT_EQ(scroll.position(), UNITS_SCROLLED_IN_ONE_SECOND);
}

/**
 * Given a paused scroll
 * When one second passes
 * Then the level has not moved
 */
TEST(LevelScroll, StaysWhereItIsWhilePaused) {
  LevelScroll scroll;
  scroll.advance(ONE_SECOND_OF_SCROLL);
  scroll.pause();

  scroll.advance(ONE_SECOND_OF_SCROLL);

  EXPECT_FLOAT_EQ(scroll.position(), UNITS_SCROLLED_IN_ONE_SECOND);
}

/**
 * Given a scroll paused then resumed
 * When one more second passes
 * Then the level moves on from where it stopped
 */
TEST(LevelScroll, MovesOnFromWhereItStoppedOnceResumed) {
  LevelScroll scroll;
  scroll.advance(ONE_SECOND_OF_SCROLL);
  scroll.pause();
  scroll.advance(ONE_SECOND_OF_SCROLL);
  scroll.resume();

  scroll.advance(ONE_SECOND_OF_SCROLL);

  EXPECT_FLOAT_EQ(scroll.position(), UNITS_SCROLLED_IN_TWO_SECONDS);
}

/**
 * Given a scroll that moves
 * When its speed is read
 * Then it is 120 logical units per second
 */
TEST(LevelScroll, TellsItsSpeedWhileItMoves) {
  const LevelScroll scroll;

  EXPECT_FLOAT_EQ(scroll.speed(), SPEED_OF_A_MOVING_SCROLL);
}

/**
 * Given a paused scroll
 * When its speed is read
 * Then it is 0
 */
TEST(LevelScroll, TellsNoSpeedWhilePaused) {
  LevelScroll scroll;
  scroll.pause();

  EXPECT_FLOAT_EQ(scroll.speed(), SPEED_OF_A_PAUSED_SCROLL);
}

/**
 * Given a level that scrolled for two seconds
 * When it is put back where it was after one second
 * Then it is 120 logical units from its start
 */
TEST(LevelScroll, ReturnsToAnEarlierPlace) {
  LevelScroll scroll;
  scroll.advance(ONE_SECOND_OF_SCROLL);
  scroll.advance(ONE_SECOND_OF_SCROLL);

  scroll.returnTo(ONE_SECOND_OF_SCROLL);

  EXPECT_FLOAT_EQ(scroll.position(), UNITS_SCROLLED_IN_ONE_SECOND);
}

/**
 * Given a level that has not scrolled yet
 * When a negative time passes
 * Then it stays at its start
 */
TEST(LevelScroll, DoesNotScrollBackwards) {
  LevelScroll scroll;

  scroll.advance(ONE_SECOND_BACKWARDS);

  EXPECT_FLOAT_EQ(scroll.position(), START_OF_THE_LEVEL);
}
