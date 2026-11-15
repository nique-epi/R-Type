#include <gtest/gtest.h>
#include "ClothGeometry.hpp"
#include "ClothGeometryTestConstants.hpp"

using rtype::client::areaOf;
using rtype::client::numberCellOf;
using rtype::client::spotOf;
using rtype::client::testing::BOX_OF_SEVENTEEN;
using rtype::client::testing::BOX_OF_THE_SECOND_DOZEN;
using rtype::client::testing::BOX_OF_THE_ZERO;
using rtype::client::testing::CORNER_OF_SEVENTEEN;
using rtype::client::testing::MIDDLE_OF_SEVENTEEN;
using rtype::client::testing::NUMBER_SEVENTEEN;
using rtype::client::testing::PLACE_OF_SEVENTEEN;
using rtype::client::testing::POINT_OF_THE_CORNER;
using rtype::client::testing::SECOND_DOZEN;

/**
 * Given the place of 17
 * When its box and its spot are read
 * Then the box is the one of 17 on the cloth, and the chips pile in its
 * middle
 */
TEST(ClothGeometry, ANumberFramesItsBoxAndPilesInItsMiddle) {
  EXPECT_EQ(areaOf(PLACE_OF_SEVENTEEN), BOX_OF_SEVENTEEN);
  EXPECT_EQ(numberCellOf(NUMBER_SEVENTEEN), BOX_OF_SEVENTEEN);
  EXPECT_EQ(spotOf(PLACE_OF_SEVENTEEN), MIDDLE_OF_SEVENTEEN);
}

/**
 * Given the corner of 17, 18, 20 and 21
 * When its spot is read
 * Then the chips pile where the two lines cross
 */
TEST(ClothGeometry, ACornerPilesWhereTheLinesCross) {
  EXPECT_EQ(spotOf(CORNER_OF_SEVENTEEN), POINT_OF_THE_CORNER);
}

/**
 * Given the zero and the second dozen
 * When their boxes are read
 * Then they are those the cloth draws
 */
TEST(ClothGeometry, TheZeroAndTheDozensFrameTheirBoxes) {
  EXPECT_EQ(numberCellOf(0), BOX_OF_THE_ZERO);
  EXPECT_EQ(areaOf(SECOND_DOZEN), BOX_OF_THE_SECOND_DOZEN);
}
