#include <gtest/gtest.h>
#include "TailGradient.hpp"
#include "TailGradientTestConstants.hpp"

using rtype::client::tailColor;

/**
 * Given a tail gradient
 * When the color at the head of the tail is asked for
 * Then it is the color of the first stop
 */
TEST(TailGradient, HeadTakesTheFirstColor) {
  EXPECT_EQ(tailColor(GRADIENT, AT_THE_HEAD), FIRST_STOP_WHITE);
}

/**
 * Given a tail gradient
 * When the color exactly at the end of a stop is asked for
 * Then it is the color of the next stop: a stop ends before its end value
 */
TEST(TailGradient, StopEndBelongsToTheNextStop) {
  EXPECT_EQ(tailColor(GRADIENT, ON_A_STOP_END), THIRD_STOP_BLUE);
}

/**
 * Given a tail gradient
 * When the color past the end of every stop is asked for
 * Then it is the color of the last stop
 */
TEST(TailGradient, PastEveryStopTakesTheLastColor) {
  EXPECT_EQ(tailColor(GRADIENT, PAST_EVERY_STOP), LAST_STOP_NAVY);
}
