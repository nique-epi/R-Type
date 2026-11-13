#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "InterfaceColorConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RowWindowTestConstants.hpp"
#include "ScrollMark.hpp"

using rtype::client::HUD_CYAN;
using rtype::client::paintScrollMark;
using rtype::client::scrollThumbOf;
using rtype::client::testing::ALL_SHOWN;
using rtype::client::testing::FROM_THE_LAST_START;
using rtype::client::testing::FROM_THE_SECOND_ROW;
using rtype::client::testing::FROM_THE_TOP;
using rtype::client::testing::TEN_ROW_TRACK;
using rtype::client::testing::THUMB_AT_THE_BOTTOM;
using rtype::client::testing::THUMB_AT_THE_TOP;
using rtype::client::testing::THUMB_ONE_ROW_DOWN;
using ::testing::Contains;

/**
 * Given a track of 159 rows for ten rows shown four at a time
 * When the rows start from the first, the second and the last start
 * Then the thumb is 63 long, at the top, 16 lower, then at the bottom
 */
TEST(ScrollMark, TheThumbFollowsTheFirstRowShown) {
  EXPECT_EQ(scrollThumbOf(TEN_ROW_TRACK, FROM_THE_TOP), THUMB_AT_THE_TOP);
  EXPECT_EQ(scrollThumbOf(TEN_ROW_TRACK, FROM_THE_SECOND_ROW),
            THUMB_ONE_ROW_DOWN);
  EXPECT_EQ(scrollThumbOf(TEN_ROW_TRACK, FROM_THE_LAST_START),
            THUMB_AT_THE_BOTTOM);
}

/**
 * Given a grid whose three rows all show
 * When its scroll mark is painted
 * Then nothing is painted: there is no more to see
 */
TEST(ScrollMark, NoMarkWhenEveryRowShows) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintScrollMark(canvas, TEN_ROW_TRACK, ALL_SHOWN);

  EXPECT_TRUE(record.fills.empty());
}

/**
 * Given ten rows shown four at a time from the second
 * When the scroll mark is painted
 * Then the thumb is filled in cyan, one row down the track
 */
TEST(ScrollMark, TheThumbIsPaintedInCyan) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintScrollMark(canvas, TEN_ROW_TRACK, FROM_THE_SECOND_ROW);

  EXPECT_THAT(record.fills, Contains(RecordedFill{.area = THUMB_ONE_ROW_DOWN,
                                                  .color = HUD_CYAN}));
}
