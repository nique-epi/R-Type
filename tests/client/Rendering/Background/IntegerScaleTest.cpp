#include <gtest/gtest.h>
#include "IntegerScale.hpp"
#include "IntegerScaleTestConstants.hpp"

using rtype::client::integerScaleFactor;

/**
 * Given a playfield 1600 x 900 pixels on screen, 3.33 times the background
 * When the factor is computed
 * Then it is three: the background is enlarged three times without smoothing
 */
TEST(IntegerScale, KeepsTheWholePartOfTheRatio) {
  EXPECT_EQ(integerScaleFactor(THREE_AND_A_THIRD_BACKGROUNDS), THREE_TIMES);
}

/**
 * Given a playfield exactly twice the background
 * When the factor is computed
 * Then it is two
 */
TEST(IntegerScale, ExactMultipleIsKept) {
  EXPECT_EQ(integerScaleFactor(TWICE_THE_BACKGROUND), TWICE);
}

/**
 * Given a playfield one pixel short of twice the background on each axis
 * When the factor is computed
 * Then it is one: the background must fit whole
 */
TEST(IntegerScale, OnePixelShortOfAMultipleFallsBack) {
  EXPECT_EQ(integerScaleFactor(JUST_UNDER_TWICE_THE_BACKGROUND), ONCE);
}

/**
 * Given a playfield four backgrounds wide but three high
 * When the factor is computed
 * Then it is three: the narrower axis decides
 */
TEST(IntegerScale, NarrowerAxisDecides) {
  EXPECT_EQ(integerScaleFactor(FOUR_WIDE_THREE_HIGH), THREE_TIMES);
}

/**
 * Given a playfield of no size, as in a minimized window
 * When the factor is computed
 * Then it is one, so the sky can still be drawn
 */
TEST(IntegerScale, EmptyPlayfieldStillGetsOne) {
  EXPECT_EQ(integerScaleFactor(NO_SIZE), ONCE);
}
