#include <gtest/gtest.h>
#include "Easing.hpp"
#include "EasingTestConstants.hpp"

using rtype::client::easeInCubic;
using rtype::client::easeInOutCubic;
using rtype::client::easeOutBack;
using rtype::client::progressBetween;
using rtype::client::progressThrough;
using rtype::client::testing::A_QUARTER;
using rtype::client::testing::AFTER_THE_SPAN;
using rtype::client::testing::BACK_OVERSHOOT;
using rtype::client::testing::BEFORE_THE_SPAN;
using rtype::client::testing::CLOCK_SPAN_LENGTH;
using rtype::client::testing::CLOCK_SPAN_START;
using rtype::client::testing::CUBE_OF_A_HALF;
using rtype::client::testing::HALF_WAY;
using rtype::client::testing::MIDDLE_OF_THE_SPAN;
using rtype::client::testing::SPAN_LENGTH;
using rtype::client::testing::SPAN_START;
using rtype::client::testing::SWAP_AT_A_QUARTER;
using rtype::client::testing::SWAP_AT_THREE_QUARTERS;
using rtype::client::testing::THREE_QUARTERS;
using rtype::client::testing::TWO_THIRDS;

/**
 * Given the cubic ease in
 * When it is read half way
 * Then it gives the cube of one half: slow first
 */
TEST(Easing, TheCubicEaseInIsSlowFirst) {
  EXPECT_FLOAT_EQ(easeInCubic(HALF_WAY), CUBE_OF_A_HALF);
}

/**
 * Given the cubic ease in and out
 * When it is read half way
 * Then it is half way too
 */
TEST(Easing, TheCubicEaseInAndOutIsHalfWayHalfWay) {
  EXPECT_FLOAT_EQ(easeInOutCubic(HALF_WAY), HALF_WAY);
}

/**
 * Given the cubic ease in and out
 * When it is read at a quarter
 * Then it gives 4 times the cube of a quarter
 */
TEST(Easing, TheCubicEaseInAndOutStartsSlow) {
  EXPECT_FLOAT_EQ(easeInOutCubic(A_QUARTER), SWAP_AT_A_QUARTER);
}

/**
 * Given the cubic ease in and out
 * When it is read at three quarters
 * Then it is as far from 1 as it was from 0 at a quarter
 */
TEST(Easing, TheCubicEaseInAndOutEndsSlow) {
  EXPECT_FLOAT_EQ(easeInOutCubic(THREE_QUARTERS), SWAP_AT_THREE_QUARTERS);
}

/**
 * Given the ease out back
 * When it is read at two thirds
 * Then it is past 1 by 1/27, its farthest
 */
TEST(Easing, TheEaseOutBackOvershootsAtTwoThirds) {
  EXPECT_FLOAT_EQ(easeOutBack(TWO_THIRDS), BACK_OVERSHOOT);
}

/**
 * Given the ease out back
 * When it is read at its start and at its end
 * Then it starts at 0 and lands on 1
 */
TEST(Easing, TheEaseOutBackStartsAtZeroAndLandsOnOne) {
  EXPECT_FLOAT_EQ(easeOutBack(0.0F), 0.0F);
  EXPECT_FLOAT_EQ(easeOutBack(1.0F), 1.0F);
}

/**
 * Given a span of one second from 1 s
 * When its progress is read at 1.5 s
 * Then it is half way
 */
TEST(Easing, AProgressIsTheShareOfTheSpanGone) {
  EXPECT_FLOAT_EQ(
      progressThrough(SPAN_START + HALF_WAY, SPAN_START, SPAN_LENGTH),
      HALF_WAY);
}

/**
 * Given a span of one second from 1 s
 * When its progress is read before it and after it
 * Then it stays 0 before and 1 after
 */
TEST(Easing, AProgressIsClampedOutsideItsSpan) {
  EXPECT_FLOAT_EQ(progressThrough(0.0F, SPAN_START, SPAN_LENGTH), 0.0F);
  EXPECT_FLOAT_EQ(
      progressThrough(SPAN_START + SPAN_LENGTH + 1.0F, SPAN_START, SPAN_LENGTH),
      1.0F);
}

/**
 * Given a span of no length at 1 s
 * When its progress is read just before 1 s and at 1 s
 * Then it is 0 before and 1 from its start
 */
TEST(Easing, ASpanOfNoLengthJumpsAtItsStart) {
  EXPECT_FLOAT_EQ(progressThrough(SPAN_START - HALF_WAY, SPAN_START, 0.0F),
                  0.0F);
  EXPECT_FLOAT_EQ(progressThrough(SPAN_START, SPAN_START, 0.0F), 1.0F);
}

/**
 * Given a span of one second from 1 s on the clock
 * When its progress is read before it, in its middle and after it
 * Then it is 0, one half, then 1
 */
TEST(Easing, AProgressOnTheClockFollowsItsSpan) {
  EXPECT_FLOAT_EQ(
      progressBetween(BEFORE_THE_SPAN, CLOCK_SPAN_START, CLOCK_SPAN_LENGTH),
      0.0F);
  EXPECT_FLOAT_EQ(
      progressBetween(MIDDLE_OF_THE_SPAN, CLOCK_SPAN_START, CLOCK_SPAN_LENGTH),
      HALF_WAY);
  EXPECT_FLOAT_EQ(
      progressBetween(AFTER_THE_SPAN, CLOCK_SPAN_START, CLOCK_SPAN_LENGTH),
      1.0F);
}

/**
 * Given a span of no length at 1 s on the clock
 * When its progress is read before 1 s and at 1 s
 * Then it is 0 before and 1 from its start
 */
TEST(Easing, ASpanOnTheClockOfNoLengthJumpsAtItsStart) {
  EXPECT_FLOAT_EQ(progressBetween(BEFORE_THE_SPAN, CLOCK_SPAN_START, {}), 0.0F);
  EXPECT_FLOAT_EQ(progressBetween(CLOCK_SPAN_START, CLOCK_SPAN_START, {}),
                  1.0F);
}
