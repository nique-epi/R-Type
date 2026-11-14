#include <gtest/gtest.h>
#include "EngineException.hpp"
#include "MaskFixtures.hpp"
#include "MaskPixel.hpp"
#include "MaskTestConstants.hpp"
#include "PixelMask.hpp"

using rtype::engine::InvalidPixelMaskException;
using rtype::engine::MaskPixel;
using rtype::engine::PixelMask;

/**
 * Given a new mask of 3 by 2 pixels
 * When its four corners are read
 * Then none is drawn: a mask starts transparent
 */
TEST(PixelMask, ANewMaskDrawsNothing) {
  const PixelMask mask(SMALL_WIDTH, SMALL_HEIGHT);

  for (const MaskPixel corner : SMALL_CORNERS) {
    EXPECT_FALSE(mask.isDrawn(corner));
  }
}

/**
 * Given a mask of 3 by 2 pixels with the middle of its bottom row drawn
 * When that pixel and its three neighbours are read
 * Then the pixel alone is drawn
 */
TEST(PixelMask, AMarkedPixelIsDrawnAndItsNeighboursAreNot) {
  PixelMask mask(SMALL_WIDTH, SMALL_HEIGHT);
  mask.markDrawn(BOTTOM_MIDDLE);

  EXPECT_TRUE(mask.isDrawn(BOTTOM_MIDDLE));
  EXPECT_FALSE(mask.isDrawn(BOTTOM_LEFT));
  EXPECT_FALSE(mask.isDrawn(BOTTOM_RIGHT));
  EXPECT_FALSE(mask.isDrawn(TOP_MIDDLE));
}

/**
 * Given a mask 100 pixels wide, two words per row, with the first pixel of
 * the second word drawn
 * When the pixels around it are read
 * Then that pixel is drawn, and neither the last pixel of the first word nor
 * the first pixel of the next row is
 */
TEST(PixelMask, APixelPastTheFirstWordIsKeptInTheNextWord) {
  PixelMask mask(WIDE_SIDE, SMALL_HEIGHT);
  mask.markDrawn(FIRST_PIXEL_OF_THE_SECOND_WORD);

  EXPECT_TRUE(mask.isDrawn(FIRST_PIXEL_OF_THE_SECOND_WORD));
  EXPECT_FALSE(mask.isDrawn(LAST_PIXEL_OF_THE_FIRST_WORD));
  EXPECT_FALSE(mask.isDrawn(FIRST_PIXEL_OF_THE_SECOND_ROW));
}

/**
 * Given no mask yet
 * When a mask 0 pixels wide is built
 * Then it is refused: a mask covers at least one pixel
 */
TEST(PixelMask, RefusesASideOfZero) {
  EXPECT_THROW(static_cast<void>(PixelMask(0, SMALL_HEIGHT)),
               InvalidPixelMaskException);
}

/**
 * Given no mask yet
 * When a mask one pixel wider than the largest side is built
 * Then it is refused
 */
TEST(PixelMask, RefusesASideAboveTheLargest) {
  EXPECT_THROW(
      static_cast<void>(PixelMask(SIDE_PAST_THE_LARGEST, SMALL_HEIGHT)),
      InvalidPixelMaskException);
}

/**
 * Given no mask yet
 * When a mask of the largest side along both axes is built
 * Then it is built, transparent
 */
TEST(PixelMask, AcceptsTheLargestSide) {
  const PixelMask mask(LARGEST_MASK_SIDE, LARGEST_MASK_SIDE);

  EXPECT_EQ(mask.width(), LARGEST_MASK_SIDE);
  EXPECT_EQ(mask.height(), LARGEST_MASK_SIDE);
}

/**
 * Given a mask of 3 by 2 pixels
 * When a pixel one column past its right edge is marked
 * Then it is refused
 */
TEST(PixelMask, RefusesToMarkAPixelPastTheRightEdge) {
  PixelMask mask(SMALL_WIDTH, SMALL_HEIGHT);

  EXPECT_THROW(mask.markDrawn(PAST_THE_RIGHT_EDGE), InvalidPixelMaskException);
}

/**
 * Given a mask of 3 by 2 pixels
 * When a pixel one row above its top edge is marked
 * Then it is refused
 */
TEST(PixelMask, RefusesToMarkAPixelAboveTheTopEdge) {
  PixelMask mask(SMALL_WIDTH, SMALL_HEIGHT);

  EXPECT_THROW(mask.markDrawn(ABOVE_THE_TOP_EDGE), InvalidPixelMaskException);
}

/**
 * Given a mask of 3 by 2 pixels, all drawn
 * When pixels just outside it are read
 * Then they read transparent: nothing is drawn outside a mask
 */
TEST(PixelMask, ReadsAPixelOutsideAsTransparent) {
  const PixelMask mask = fullMask(SMALL_WIDTH, SMALL_HEIGHT);

  EXPECT_FALSE(mask.isDrawn(LEFT_OF_THE_LEFT_EDGE));
  EXPECT_FALSE(mask.isDrawn(PAST_THE_RIGHT_EDGE));
}

/**
 * Given a row of four pixels drawn at both ends
 * When the whole row is read
 * Then it reads 1001: the first pixel in the highest bit of the span
 */
TEST(PixelMask, BitsOfASpanReadLeftToRight) {
  const PixelMask mask = maskFromRows(ROW_DRAWN_AT_BOTH_ENDS);

  EXPECT_EQ(mask.bitsOf(WHOLE_ROW_OF_FOUR), BOTH_ENDS_OF_FOUR);
}

/**
 * Given a row of four pixels drawn at both ends
 * When its two inner pixels are read
 * Then they read nothing
 */
TEST(PixelMask, BitsOfAnInnerSpanLeaveTheEndsOut) {
  const PixelMask mask = maskFromRows(ROW_DRAWN_AT_BOTH_ENDS);

  EXPECT_EQ(mask.bitsOf(INNER_TWO_OF_FOUR), NO_PIXEL);
}

/**
 * Given a mask 100 pixels wide with the last pixel of the first word and the
 * first pixel of the second word drawn
 * When the four pixels from column 62 are read
 * Then they read 0110: a span is read across the two words that hold it
 */
TEST(PixelMask, BitsOfASpanAcrossTwoWords) {
  PixelMask mask(WIDE_SIDE, SMALL_HEIGHT);
  mask.markDrawn(LAST_PIXEL_OF_THE_FIRST_WORD);
  mask.markDrawn(FIRST_PIXEL_OF_THE_SECOND_WORD);

  EXPECT_EQ(mask.bitsOf(SPAN_ACROSS_TWO_WORDS), MIDDLE_TWO_OF_FOUR);
}

/**
 * Given a mask of 3 by 2 pixels
 * When a span of no pixel is read
 * Then it is refused
 */
TEST(PixelMask, RefusesASpanOfNoPixel) {
  const PixelMask mask(SMALL_WIDTH, SMALL_HEIGHT);

  EXPECT_THROW(static_cast<void>(mask.bitsOf(SPAN_OF_NO_PIXEL)),
               InvalidPixelMaskException);
}

/**
 * Given a mask 100 pixels wide
 * When a span of one pixel more than a word is read
 * Then it is refused: a span fits in the bits of one word
 */
TEST(PixelMask, RefusesASpanPastAWord) {
  const PixelMask mask(WIDE_SIDE, SMALL_HEIGHT);

  EXPECT_THROW(static_cast<void>(mask.bitsOf(SPAN_PAST_A_WORD)),
               InvalidPixelMaskException);
}

/**
 * Given a mask of 3 by 2 pixels
 * When the two pixels from its last column are read
 * Then it is refused: the second lies past the row
 */
TEST(PixelMask, RefusesASpanPastTheRow) {
  const PixelMask mask(SMALL_WIDTH, SMALL_HEIGHT);

  EXPECT_THROW(static_cast<void>(mask.bitsOf(SPAN_PAST_THE_ROW)),
               InvalidPixelMaskException);
}
