#include <gtest/gtest.h>
#include "InterfacePixel.hpp"
#include "InterfacePixelTestConstants.hpp"

using rtype::client::interfacePixelAt;
using rtype::client::testing::FIRST_PIXEL;
using rtype::client::testing::LAST_POINT_OF_THE_FIRST_PIXEL;
using rtype::client::testing::PIXEL_LEFT_OF_THE_PICTURE;
using rtype::client::testing::PIXEL_ONE_TWO;
using rtype::client::testing::POINT_LEFT_OF_THE_PICTURE;
using rtype::client::testing::POINT_OF_PIXEL_ONE_TWO;

/**
 * Given the last point of the playfield inside the first pixel
 * When its pixel is asked
 * Then it is the first pixel
 */
TEST(InterfacePixel, APointBelongsToThePixelItLiesIn) {
  EXPECT_EQ(interfacePixelAt(LAST_POINT_OF_THE_FIRST_PIXEL), FIRST_PIXEL);
}

/**
 * Given a point at the top-left corner of pixel (1, 2)
 * When its pixel is asked
 * Then it is (1, 2)
 */
TEST(InterfacePixel, ACornerBelongsToThePixelItStarts) {
  EXPECT_EQ(interfacePixelAt(POINT_OF_PIXEL_ONE_TWO), PIXEL_ONE_TWO);
}

/**
 * Given a point just left of and above the picture
 * When its pixel is asked
 * Then the pixel is negative, outside every control
 */
TEST(InterfacePixel, APointLeftOfThePictureGivesANegativePixel) {
  EXPECT_EQ(interfacePixelAt(POINT_LEFT_OF_THE_PICTURE),
            PIXEL_LEFT_OF_THE_PICTURE);
}
