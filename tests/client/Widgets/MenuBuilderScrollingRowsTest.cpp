#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include "Menu.hpp"
#include "MenuBuilderScrollingRowsFixtures.hpp"
#include "MenuBuilderScrollingRowsTestConstants.hpp"
#include "MenuPainting.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingRowControl.hpp"

using rtype::client::Menu;
using rtype::client::testing::FIRST_DESCRIPTION_LINE;
using rtype::client::testing::FIRST_DESCRIPTION_LINE_AT_LARGEST;
using rtype::client::testing::FIRST_SHOWN_ROW_LABEL;
using rtype::client::testing::FOURTH_ROW_LABEL_AT_LARGEST;
using rtype::client::testing::INSIDE_THE_FIRST_SHOWN_ROW;
using rtype::client::testing::LARGEST_TEXT;
using rtype::client::testing::LAST_SHOWN_ROW_LABEL;
using rtype::client::testing::ONE_NOTCH_DOWN;
using rtype::client::testing::PRESSES_TO_THE_NINTH_ROW;
using rtype::client::testing::SECOND_DESCRIPTION_LINE;
using rtype::client::testing::SECOND_DESCRIPTION_LINE_AT_LARGEST;
using rtype::client::testing::SECOND_ROW;

/**
 * Given a tabbed panel of nine rows, one more than it shows
 * When it is painted
 * Then the eighth row is the last written, and the ninth is not written
 */
TEST(MenuBuilderScrollingRows, APanelShowsItsFirstEightRows) {
  RowControlRecord record;
  const InterfaceCanvasRecord painting = paintingOf(panelOfNineRows(record));

  EXPECT_EQ(anchorOf(painting, "ROW 8"), LAST_SHOWN_ROW_LABEL);
  EXPECT_EQ(writingOf(painting, "ROW 9"), nullptr);
}

/**
 * Given the same panel, its first row selected
 * When Down is pressed until the ninth row is selected
 * Then the rows scroll by one: the ninth is written where the eighth was
 */
TEST(MenuBuilderScrollingRows, SelectingAHiddenRowScrollsToIt) {
  RowControlRecord record;
  Menu menu = panelOfNineRows(record);

  for (std::size_t press = 0; press < PRESSES_TO_THE_NINTH_ROW; ++press) {
    menu.handleEvent(downPress(), wholePlayfield());
  }

  EXPECT_EQ(anchorOf(paintingOf(menu), "ROW 9"), LAST_SHOWN_ROW_LABEL);
}

/**
 * Given the same panel, its first row selected
 * When the wheel turns one notch toward the player over the rows
 * Then the second row is written where the first was, and the first is not
 */
TEST(MenuBuilderScrollingRows, TheWheelScrollsTheRows) {
  RowControlRecord record;
  Menu menu = panelOfNineRows(record);

  menu.handleEvent(wheelTurn(ONE_NOTCH_DOWN, INSIDE_THE_FIRST_SHOWN_ROW),
                   wholePlayfield());

  const InterfaceCanvasRecord painting = paintingOf(menu);
  EXPECT_EQ(anchorOf(painting, "ROW 2"), FIRST_SHOWN_ROW_LABEL);
  EXPECT_EQ(writingOf(painting, "ROW 1"), nullptr);
}

/**
 * Given the same panel scrolled by one row
 * When the pointer moves over the first shown row
 * Then it selects the second row, the one shown there
 */
TEST(MenuBuilderScrollingRows, ThePointerSelectsTheRowShownUnderIt) {
  RowControlRecord record;
  Menu menu = panelOfNineRows(record);
  menu.handleEvent(wheelTurn(ONE_NOTCH_DOWN, INSIDE_THE_FIRST_SHOWN_ROW),
                   wholePlayfield());

  menu.handleEvent(pointerMove(INSIDE_THE_FIRST_SHOWN_ROW * INTERFACE_PIXEL),
                   wholePlayfield());

  EXPECT_EQ(menu.selectedButton(), std::optional<std::size_t>(SECOND_ROW));
}

/**
 * Given the panel of nine rows laid out at the largest text size
 * When it is painted at that size
 * Then four rows show, the fourth written at (36, 168), the fifth not at all
 */
TEST(MenuBuilderScrollingRows, AtTheLargestSizeAPanelShowsFourRows) {
  RowControlRecord record;
  const InterfaceCanvasRecord painting =
      paintingAt(panelOfNineRowsAt(record, LARGEST_TEXT), LARGEST_TEXT);

  EXPECT_EQ(anchorOf(painting, "ROW 4"), FOURTH_ROW_LABEL_AT_LARGEST);
  EXPECT_EQ(writingOf(painting, "ROW 5"), nullptr);
}

/**
 * Given the same panel, its first row described in 36 letters
 * When it is painted at the largest size
 * Then the description takes two lines right of BACK, each word whole
 */
TEST(MenuBuilderScrollingRows, AtTheLargestSizeTheDescriptionWraps) {
  RowControlRecord record;
  const InterfaceCanvasRecord painting =
      paintingAt(panelOfNineRowsAt(record, LARGEST_TEXT), LARGEST_TEXT);

  EXPECT_EQ(anchorOf(painting, FIRST_DESCRIPTION_LINE),
            FIRST_DESCRIPTION_LINE_AT_LARGEST);
  EXPECT_EQ(anchorOf(painting, SECOND_DESCRIPTION_LINE),
            SECOND_DESCRIPTION_LINE_AT_LARGEST);
}
