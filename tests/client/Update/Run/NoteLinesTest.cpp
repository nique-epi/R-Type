#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "NoteLines.hpp"
#include "RunTestConstants.hpp"

using rtype::client::noteLinesOf;
using rtype::client::testing::ACCENTED_NOTE;
using rtype::client::testing::CUT_NOTE;
using rtype::client::testing::LAST_NOTE_SHOWN;
using rtype::client::testing::LONG_NOTE;
using rtype::client::testing::NINE_NOTES;
using rtype::client::testing::NOTE_LINES_OF_THE_PANEL;
using rtype::client::testing::TAG_FIRST_NOTE;
using rtype::client::testing::TAG_NOTES;
using rtype::client::testing::TAG_SECOND_NOTE;
using rtype::client::testing::TAG_THIRD_NOTE;
using rtype::client::testing::UNACCENTED_NOTE;

/**
 * Given the notes of a tag with a heading, a blank line and two list items
 * When the lines of the panel are made of them
 * Then each line of text is kept, without its list mark or trailing blanks
 */
TEST(NoteLines, KeepsEachLineOfTextWithoutItsListMark) {
  EXPECT_EQ(noteLinesOf(TAG_NOTES),
            (std::vector<std::string>{std::string{TAG_FIRST_NOTE},
                                      std::string{TAG_SECOND_NOTE},
                                      std::string{TAG_THIRD_NOTE}}));
}

/**
 * Given a note longer than the panel
 * When the lines of the panel are made of it
 * Then it is cut to the width of the panel
 */
TEST(NoteLines, CutsANoteToTheWidthOfThePanel) {
  EXPECT_EQ(noteLinesOf(LONG_NOTE),
            std::vector<std::string>{std::string{CUT_NOTE}});
}

/**
 * Given a note holding a character the font does not draw
 * When the lines of the panel are made of it
 * Then that character is left out
 */
TEST(NoteLines, LeavesOutWhatTheFontDoesNotDraw) {
  EXPECT_EQ(noteLinesOf(ACCENTED_NOTE),
            std::vector<std::string>{std::string{UNACCENTED_NOTE}});
}

/**
 * Given nine notes
 * When the lines of the panel are made of them
 * Then the eight the panel holds are kept, the ninth left out
 */
TEST(NoteLines, KeepsTheLinesThePanelHolds) {
  const std::vector<std::string> lines = noteLinesOf(NINE_NOTES);

  ASSERT_EQ(lines.size(), NOTE_LINES_OF_THE_PANEL);
  EXPECT_EQ(lines.back(), LAST_NOTE_SHOWN);
}
