#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "BoxCorners.hpp"
#include "BoxCornersTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::paintBoxCorners;
using rtype::client::testing::BOTTOM_LEFT_ACROSS;
using rtype::client::testing::BOTTOM_LEFT_UP;
using rtype::client::testing::BOTTOM_RIGHT_ACROSS;
using rtype::client::testing::BOTTOM_RIGHT_UP;
using rtype::client::testing::CLOSED_BOX;
using rtype::client::testing::CORNER_TEST_COLOR;
using rtype::client::testing::CORNERED_BOX;
using rtype::client::testing::TOP_LEFT_ACROSS;
using rtype::client::testing::TOP_LEFT_DOWN;
using rtype::client::testing::TOP_RIGHT_ACROSS;
using rtype::client::testing::TOP_RIGHT_DOWN;
using ::testing::UnorderedElementsAre;

/**
 * Given a box 30 pixels wide and 40 tall at (10, 20)
 * When its corners are painted
 * Then each corner is two strokes 8 pixels long on its edges, in the color
 */
TEST(BoxCorners, EachCornerIsTwoStrokesOnTheEdges) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintBoxCorners(canvas, CORNERED_BOX, CORNER_TEST_COLOR);

  EXPECT_THAT(
      record.fills,
      UnorderedElementsAre(
          RecordedFill{.area = TOP_LEFT_ACROSS, .color = CORNER_TEST_COLOR},
          RecordedFill{.area = TOP_LEFT_DOWN, .color = CORNER_TEST_COLOR},
          RecordedFill{.area = TOP_RIGHT_ACROSS, .color = CORNER_TEST_COLOR},
          RecordedFill{.area = TOP_RIGHT_DOWN, .color = CORNER_TEST_COLOR},
          RecordedFill{.area = BOTTOM_LEFT_ACROSS, .color = CORNER_TEST_COLOR},
          RecordedFill{.area = BOTTOM_LEFT_UP, .color = CORNER_TEST_COLOR},
          RecordedFill{.area = BOTTOM_RIGHT_ACROSS, .color = CORNER_TEST_COLOR},
          RecordedFill{.area = BOTTOM_RIGHT_UP, .color = CORNER_TEST_COLOR}));
}

/**
 * Given a closed box, no row tall, at row 20
 * When its corners are painted
 * Then its bottom strokes lie on row 20 with its top ones, never above it
 */
TEST(BoxCorners, AClosedBoxPaintsItsBottomStrokesOnItsTopRow) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintBoxCorners(canvas, CLOSED_BOX, CORNER_TEST_COLOR);

  for (const RecordedFill& fill : record.fills) {
    if (fill.area.size.y == 1) {
      EXPECT_EQ(fill.area.position.y, CLOSED_BOX.position.y);
    }
  }
}
