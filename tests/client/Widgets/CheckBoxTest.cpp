#include <gtest/gtest.h>
#include "BackgroundColorConstants.hpp"
#include "CheckBox.hpp"
#include "CheckMark.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::CheckBox;
using rtype::client::CheckMark;
using rtype::client::STAR_WHITE;

/**
 * Given a checked box
 * When it is painted
 * Then its inside is filled but for a margin of 2 pixels
 */
TEST(CheckBoxTest, ACheckedBoxIsFilledInsideItsMargin) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  CheckBox({0, 0}, CheckMark::Checked).paint(canvas, STAR_WHITE);
  const auto pixels = litPixelsOf(record);
  EXPECT_TRUE(pixels.contains({3, 3}));
  EXPECT_FALSE(pixels.contains({1, 1}));
}

/**
 * Given the box of a choice that cannot be taken
 * When it is painted
 * Then it is dashed: its border is not whole
 */
TEST(CheckBoxTest, AChoiceThatCannotBeTakenHasADashedBox) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  CheckBox({0, 0}, CheckMark::Unavailable).paint(canvas, STAR_WHITE);
  EXPECT_EQ(litPixelsOf(record).size(), 12U);
}
