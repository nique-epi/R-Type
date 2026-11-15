#include <gtest/gtest.h>
#include "TextMeasures.hpp"
#include "TextMeasuresTestConstants.hpp"

using rtype::client::halfPixelsOf;
using rtype::client::lineHeightOf;
using rtype::client::regularWidthOf;
using rtype::client::widthAtSizeOf;
using rtype::client::testing::FIVE_LETTERS;
using rtype::client::testing::FIVE_LETTERS_AT_LARGEST;
using rtype::client::testing::FIVE_LETTERS_WITH_AN_ACCENT;
using rtype::client::testing::LARGE_LINE_HEIGHT;
using rtype::client::testing::LARGE_TEXT;
using rtype::client::testing::LARGEST_LINE_HEIGHT;
using rtype::client::testing::LARGEST_TEXT;
using rtype::client::testing::NORMAL_LINE_HEIGHT;
using rtype::client::testing::NORMAL_TEXT;
using rtype::client::testing::PLAIN_SCALE;
using rtype::client::testing::WARNING_HALF_PIXELS;
using rtype::client::testing::WARNING_SCALE;
using rtype::client::testing::WIDEST_SMALL_LETTER;
using rtype::client::testing::WIDEST_SMALL_LETTER_AT_LARGE;

/**
 * Given the three text sizes
 * When the height of a line is measured at each
 * Then it is 8, 12 and 16 pixels: the font, half again, twice
 */
TEST(TextMeasures, ALineGrowsWithTheSize) {
  EXPECT_EQ(lineHeightOf(NORMAL_TEXT), NORMAL_LINE_HEIGHT);
  EXPECT_EQ(lineHeightOf(LARGE_TEXT), LARGE_LINE_HEIGHT);
  EXPECT_EQ(lineHeightOf(LARGEST_TEXT), LARGEST_LINE_HEIGHT);
}

/**
 * Given a word of five letters in the regular font
 * When it is measured at the largest size
 * Then it is five lines wide, 80 pixels
 */
TEST(TextMeasures, ARegularWordIsALinePerLetter) {
  EXPECT_EQ(regularWidthOf(LARGEST_TEXT, FIVE_LETTERS),
            FIVE_LETTERS_AT_LARGEST);
}

/**
 * Given a word of five letters, one of them accented, two bytes in UTF-8
 * When it is measured at the largest size
 * Then it is as wide as a word of five plain letters
 */
TEST(TextMeasures, AnAccentedLetterCountsOnce) {
  EXPECT_EQ(regularWidthOf(LARGEST_TEXT, FIVE_LETTERS_WITH_AN_ACCENT),
            FIVE_LETTERS_AT_LARGEST);
}

/**
 * Given a line of an odd width as drawn
 * When it is measured half again as large
 * Then its width is rounded up, so a box around it never cuts its last pixel
 */
TEST(TextMeasures, AHalfPixelIsRoundedUp) {
  EXPECT_EQ(widthAtSizeOf(LARGE_TEXT, WIDEST_SMALL_LETTER),
            WIDEST_SMALL_LETTER_AT_LARGE);
}

/**
 * Given the WARNING banner, written four times as large
 * When the player chose the largest text
 * Then it keeps its own size, not twice of it
 */
TEST(TextMeasures, ALargerStyleKeepsItsOwnSize) {
  EXPECT_EQ(halfPixelsOf(LARGEST_TEXT, WARNING_SCALE), WARNING_HALF_PIXELS);
}

/**
 * Given a plain line
 * When the player chose the largest text
 * Then it is written at the player's size
 */
TEST(TextMeasures, APlainLineTakesThePlayersSize) {
  EXPECT_EQ(halfPixelsOf(LARGEST_TEXT, PLAIN_SCALE),
            LARGEST_TEXT.halfPixelsPerFontPixel);
}
