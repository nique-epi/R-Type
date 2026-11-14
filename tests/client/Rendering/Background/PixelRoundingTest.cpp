#include <gtest/gtest.h>
#include "PixelRounding.hpp"
#include "PixelRoundingTestConstants.hpp"

using rtype::client::enlargedPixels;
using rtype::client::shiftPastPixel;

/**
 * Given a position a quarter of a pixel right of column 2
 * When its shift past its pixel is asked for
 * Then it is a quarter
 */
TEST(PixelRounding, ShiftsAPositionByWhatLiesPastItsPixel) {
  EXPECT_FLOAT_EQ(shiftPastPixel(A_QUARTER_PAST_TWO), A_QUARTER_OF_A_PIXEL);
}

/**
 * Given a position a quarter of a pixel left of column 0
 * When its shift past its pixel is asked for
 * Then it is three quarters: it lies in the pixel of column -1
 */
TEST(PixelRounding, ShiftsANegativePositionPastThePixelLeftOfIt) {
  EXPECT_FLOAT_EQ(shiftPastPixel(A_QUARTER_SHORT_OF_ZERO), THREE_QUARTERS);
}

/**
 * Given a position on column 3
 * When its shift past its pixel is asked for
 * Then it is none
 */
TEST(PixelRounding, ShiftsNothingOnAWholeColumn) {
  EXPECT_FLOAT_EQ(shiftPastPixel(ON_THREE), NOTHING);
}

/**
 * Given three quarters of a background pixel to the left
 * When they are counted on the background enlarged four times
 * Then they cover three of its pixels to the left
 */
TEST(PixelRounding, CountsAShiftInPixelsOfTheEnlargedBackground) {
  EXPECT_EQ(enlargedPixels(THREE_QUARTERS_LEFT, FOUR_TIMES),
            THREE_ENLARGED_PIXELS_LEFT);
}

/**
 * Given 0.3 of a background pixel to the right
 * When they are counted on the background enlarged twice
 * Then they round to one of its pixels
 */
TEST(PixelRounding, RoundsAShiftToTheNearestEnlargedPixel) {
  EXPECT_EQ(enlargedPixels(THREE_TENTHS_RIGHT, TWICE),
            ONE_ENLARGED_PIXEL_RIGHT);
}
