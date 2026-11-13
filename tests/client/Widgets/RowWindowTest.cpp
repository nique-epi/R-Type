#include <gtest/gtest.h>
#include "RowWindow.hpp"
#include "RowWindowTestConstants.hpp"

using rtype::client::rowsScrolledBy;
using rtype::client::rowsShowing;
using rtype::client::RowWindow;
using rtype::client::showsRow;
using rtype::client::testing::FIFTH_ROW;
using rtype::client::testing::FIRST_ROW;
using rtype::client::testing::FOURTH_ROW;
using rtype::client::testing::FROM_THE_LAST_START;
using rtype::client::testing::FROM_THE_SECOND_ROW;
using rtype::client::testing::FROM_THE_TOP;
using rtype::client::testing::LAST_ROW;
using rtype::client::testing::ONE_ROW_DOWN;
using rtype::client::testing::ONE_ROW_UP;

/**
 * Given ten rows shown four at a time from the first
 * When the fifth row must show
 * Then the rows start from the second: just far enough
 */
TEST(RowWindow, TheRowsFollowARowBelowThem) {
  EXPECT_EQ(rowsShowing(FROM_THE_TOP, FIFTH_ROW), FROM_THE_SECOND_ROW);
}

/**
 * Given the rows starting from the seventh
 * When the fourth row must show
 * Then the rows start from the fourth
 */
TEST(RowWindow, TheRowsFollowARowAboveThem) {
  EXPECT_EQ(rowsShowing(FROM_THE_LAST_START, FOURTH_ROW).first, FOURTH_ROW);
}

/**
 * Given the rows starting from the first, the fourth among them
 * When the fourth row must show
 * Then the rows do not move
 */
TEST(RowWindow, ARowAlreadyShownMovesNothing) {
  EXPECT_EQ(rowsShowing(FROM_THE_TOP, FOURTH_ROW), FROM_THE_TOP);
}

/**
 * Given the rows at the top, and at the last row they start from
 * When they scroll up from the top and down from the bottom
 * Then they stay where they are
 */
TEST(RowWindow, ScrollingStopsAtBothEnds) {
  EXPECT_EQ(rowsScrolledBy(FROM_THE_TOP, ONE_ROW_UP), FROM_THE_TOP);
  EXPECT_EQ(rowsScrolledBy(FROM_THE_LAST_START, ONE_ROW_DOWN),
            FROM_THE_LAST_START);
}

/**
 * Given the rows at the top
 * When they scroll one row down
 * Then they start from the second row, and no longer show the first
 */
TEST(RowWindow, ScrollingDownHidesTheFirstRow) {
  const RowWindow scrolled = rowsScrolledBy(FROM_THE_TOP, ONE_ROW_DOWN);

  EXPECT_EQ(scrolled, FROM_THE_SECOND_ROW);
  EXPECT_FALSE(showsRow(scrolled, FIRST_ROW));
  EXPECT_TRUE(showsRow(scrolled, FIFTH_ROW));
}

/**
 * Given the rows at the top
 * When the last row must show
 * Then the rows start from the seventh, the last start
 */
TEST(RowWindow, TheLastRowShowsFromTheLastStart) {
  EXPECT_EQ(rowsShowing(FROM_THE_TOP, LAST_ROW), FROM_THE_LAST_START);
}
