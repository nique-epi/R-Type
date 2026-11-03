#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <optional>
#include <set>
#include <utility>
#include "InkTagConstants.hpp"
#include "ItemListFixtures.hpp"
#include "ItemListTestConstants.hpp"
#include "ItemRow.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TextStyle.hpp"

using rtype::client::INK_TAG_COLOR;
using rtype::client::ItemRow;
using rtype::client::TextAlignment;
using rtype::client::TextFont;
using rtype::client::testing::ACE_TAG;
using rtype::client::testing::ACE_TAG_WORD;
using rtype::client::testing::CELL_COLOR;
using rtype::client::testing::FIRST_ROW_FIRST_CELL;
using rtype::client::testing::FIRST_ROW_HIGHLIGHT;
using rtype::client::testing::FIRST_ROW_SECOND_CELL;
using rtype::client::testing::HIGHLIGHT_COLOR;
using rtype::client::testing::NOTE_ANCHOR;
using rtype::client::testing::PIN_LINE_FILL_COUNT;
using rtype::client::testing::PIN_LINE_ROW;
using rtype::client::testing::PINNED_ROW_SECOND_CELL;
using rtype::client::testing::SECOND_ROW_SECOND_CELL;
using ::testing::Contains;
using ::testing::IsEmpty;

/**
 * Given a list without a row
 * When it is painted
 * Then it paints nothing
 */
TEST(ItemList, AnEmptyListPaintsNothing) {
  const InterfaceCanvasRecord record = paintedList({});

  EXPECT_THAT(record.writings, IsEmpty());
  EXPECT_THAT(record.fills, IsEmpty());
}

/**
 * Given a row of two cells
 * When it is painted
 * Then each cell is anchored on its column, on the side of that column
 */
TEST(ItemList, ACellFollowsItsColumn) {
  const InterfaceCanvasRecord record = paintedList({rowNamed("ACE")});

  const RecordedWriting* rank = writingOf(record, "1");
  ASSERT_NE(rank, nullptr);
  EXPECT_EQ(rank->anchor, FIRST_ROW_FIRST_CELL);
  EXPECT_EQ(rank->style.alignment, TextAlignment::Right);
  EXPECT_EQ(anchorOf(record, "ACE"), FIRST_ROW_SECOND_CELL);
}

/**
 * Given two rows, as many as the list shows
 * When they are painted
 * Then the second one stands a pitch under the first
 */
TEST(ItemList, RowsStandAPitchApart) {
  const InterfaceCanvasRecord record =
      paintedList({rowNamed("ACE"), rowNamed("KIRA")});

  EXPECT_EQ(anchorOf(record, "KIRA"), SECOND_ROW_SECOND_CELL);
}

/**
 * Given one row more than the list shows
 * When they are painted
 * Then the last one is left out
 */
TEST(ItemList, ARowPastTheCapacityIsLeftOut) {
  const InterfaceCanvasRecord record =
      paintedList({rowNamed("ACE"), rowNamed("KIRA"), rowNamed("SOL")});

  EXPECT_EQ(writingOf(record, "SOL"), nullptr);
}

/**
 * Given a row with a highlight
 * When it is painted
 * Then its color is filled past the sides of the row
 */
TEST(ItemList, AHighlightIsFilledBehindItsRow) {
  ItemRow row = rowNamed("ACE");
  row.highlight = HIGHLIGHT_COLOR;

  const InterfaceCanvasRecord record = paintedList({std::move(row)});

  EXPECT_THAT(record.fills, Contains(RecordedFill{.area = FIRST_ROW_HIGHLIGHT,
                                                  .color = HIGHLIGHT_COLOR}));
}

/**
 * Given a row and a pinned row
 * When they are painted
 * Then the pinned row stands under a dashed line, under the rows
 */
TEST(ItemList, APinnedRowStandsUnderADashedLine) {
  const InterfaceCanvasRecord record =
      paintedList({rowNamed("ACE")}, rowNamed("KIRA"));

  EXPECT_EQ(anchorOf(record, "KIRA"), PINNED_ROW_SECOND_CELL);
  const std::set<std::pair<int, int>> lit = litPixelsOf(record);
  EXPECT_TRUE(lit.contains({FIRST_ROW_HIGHLIGHT.position.x, PIN_LINE_ROW}));
  EXPECT_FALSE(
      lit.contains({FIRST_ROW_HIGHLIGHT.position.x + 1, PIN_LINE_ROW}));
}

/**
 * Given a row and a pinned row, none with a highlight or a tag
 * When they are painted
 * Then the only fills are one side of the line and its two ends: the line is
 * one pixel tall, so its top side and its bottom side are the same pixels
 */
TEST(ItemList, ALineOnePixelTallFillsOneSideOnly) {
  const InterfaceCanvasRecord record =
      paintedList({rowNamed("ACE")}, rowNamed("KIRA"));

  EXPECT_EQ(record.fills.size(), PIN_LINE_FILL_COUNT);
}

/**
 * Given a row and a note, and no pinned row
 * When they are painted
 * Then the note is centered under the rows, past the dashed line
 */
TEST(ItemList, ANoteStandsUnderADashedLine) {
  const InterfaceCanvasRecord record = paintedListWithNote("NO SCORE");

  const RecordedWriting* note = writingOf(record, "NO SCORE");
  ASSERT_NE(note, nullptr);
  EXPECT_EQ(note->anchor, NOTE_ANCHOR);
  EXPECT_EQ(note->style.alignment, TextAlignment::Center);
  EXPECT_TRUE(litPixelsOf(record).contains(
      {FIRST_ROW_HIGHLIGHT.position.x, PIN_LINE_ROW}));
}

/**
 * Given a row and no pinned row
 * When they are painted
 * Then no line is drawn
 */
TEST(ItemList, NoPinnedRowDrawsNoLine) {
  const InterfaceCanvasRecord record = paintedList({rowNamed("ACE")});

  EXPECT_THAT(record.fills, IsEmpty());
}

/**
 * Given a cell with a tag
 * When it is painted
 * Then the word stands on ink right after its text, in the color of the cell
 */
TEST(ItemList, ATagFollowsItsCell) {
  ItemRow row = rowNamed("ACE");
  row.cells.back().tag = "YOU";

  const InterfaceCanvasRecord record = paintedList({std::move(row)});

  EXPECT_THAT(record.fills,
              Contains(RecordedFill{.area = ACE_TAG, .color = INK_TAG_COLOR}));
  const RecordedWriting* word = writingOf(record, "YOU");
  ASSERT_NE(word, nullptr);
  EXPECT_EQ(word->anchor, ACE_TAG_WORD);
  EXPECT_EQ(word->style.font, TextFont::Small);
  EXPECT_EQ(word->style.color, CELL_COLOR);
}
