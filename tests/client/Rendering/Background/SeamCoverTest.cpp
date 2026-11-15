#include <gtest/gtest.h>
#include "SeamCoverFixtures.hpp"
#include "SeamCoverTestConstants.hpp"

/**
 * Given a seam
 * When the column left of it is read
 * Then every pixel of it is hidden
 */
TEST(SeamCover, HidesEveryPixelLeftOfTheSeam) {
  EXPECT_EQ(hiddenRowsOf(LEFT_OF_THE_SEAM), EVERY_ROW);
}

/**
 * Given a seam
 * When the first column of its edge is read
 * Then every pixel of it is hidden still
 */
TEST(SeamCover, HidesTheWholeFirstColumnOfTheEdge) {
  EXPECT_EQ(hiddenRowsOf(FIRST_COLUMN_OF_THE_EDGE), EVERY_ROW);
}

/**
 * Given a seam
 * When the column halfway through its edge is read
 * Then half its pixels are hidden
 */
TEST(SeamCover, HidesHalfTheEdgeHalfwayThrough) {
  EXPECT_EQ(hiddenRowsOf(HALFWAY_THROUGH_THE_EDGE), HALF_THE_ROWS);
}

/**
 * Given a seam
 * When the last column of its edge is read
 * Then none of its pixels is hidden
 */
TEST(SeamCover, ShowsTheLastColumnOfTheEdge) {
  EXPECT_EQ(hiddenRowsOf(LAST_COLUMN_OF_THE_EDGE), NO_ROW);
}

/**
 * Given a seam
 * When the column right after its edge is read
 * Then none of its pixels is hidden
 */
TEST(SeamCover, ShowsEveryPixelPastTheEdge) {
  EXPECT_EQ(hiddenRowsOf(PAST_THE_EDGE), NO_ROW);
}
