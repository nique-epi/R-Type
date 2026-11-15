#include <gtest/gtest.h>
#include "MaskFixtures.hpp"
#include "MaskOverlap.hpp"
#include "MaskTestConstants.hpp"
#include "PixelMask.hpp"
#include "PlacedMask.hpp"

using rtype::engine::maskOverlapsArea;
using rtype::engine::masksOverlap;
using rtype::engine::PixelMask;
using rtype::engine::PlacedMask;

/**
 * Given the ship of entities.png at 0, 0 and the bullet of an enemy whose
 * frame overlaps the frame of the ship over the empty corner past its top fin
 * When the masks are compared
 * Then they do not touch: the rectangles overlap, no drawn pixel does
 */
TEST(MaskOverlap, ABulletInTheGapOfTheShipTouchesNothing) {
  const PixelMask ship = maskFromRows(SHIP_ROWS);
  const PixelMask bullet = maskFromRows(BULLET_ROWS);

  EXPECT_FALSE(masksOverlap(PlacedMask{.mask = &ship, .left = 0, .top = 0},
                            PlacedMask{.mask = &bullet,
                                       .left = BULLET_IN_THE_GAP_LEFT,
                                       .top = BULLET_IN_THE_GAP_TOP}));
}

/**
 * Given the ship at 0, 0 and the bullet of an enemy over the tip of its right
 * wing
 * When the masks are compared
 * Then they touch
 */
TEST(MaskOverlap, ABulletOnTheWingTouchesTheShip) {
  const PixelMask ship = maskFromRows(SHIP_ROWS);
  const PixelMask bullet = maskFromRows(BULLET_ROWS);

  EXPECT_TRUE(masksOverlap(PlacedMask{.mask = &ship, .left = 0, .top = 0},
                           PlacedMask{.mask = &bullet,
                                      .left = BULLET_ON_THE_WING_LEFT,
                                      .top = BULLET_ON_THE_WING_TOP}));
}

/**
 * Given two full squares of 2 by 2, the second starting at the column where
 * the first ends
 * When they are compared
 * Then they do not touch: masks side by side share no pixel
 */
TEST(MaskOverlap, MasksSideBySideDoNotTouch) {
  const PixelMask square = fullMask(SQUARE_SIDE, SQUARE_SIDE);

  EXPECT_FALSE(masksOverlap(
      PlacedMask{.mask = &square, .left = 0, .top = 0},
      PlacedMask{.mask = &square, .left = SQUARE_BESIDE_LEFT, .top = 0}));
}

/**
 * Given two full squares of 2 by 2 sharing one column
 * When they are compared
 * Then they touch
 */
TEST(MaskOverlap, MasksSharingOneColumnTouch) {
  const PixelMask square = fullMask(SQUARE_SIDE, SQUARE_SIDE);

  EXPECT_TRUE(masksOverlap(
      PlacedMask{.mask = &square, .left = 0, .top = 0},
      PlacedMask{
          .mask = &square, .left = SQUARE_SHARING_A_COLUMN_LEFT, .top = 0}));
}

/**
 * Given two full squares, the second far below the first
 * When they are compared
 * Then they do not touch: they share no row
 */
TEST(MaskOverlap, MasksSharingNoRowDoNotTouch) {
  const PixelMask square = fullMask(SQUARE_SIDE, SQUARE_SIDE);

  EXPECT_FALSE(masksOverlap(
      PlacedMask{.mask = &square, .left = 0, .top = 0},
      PlacedMask{.mask = &square, .left = 0, .top = SQUARE_BELOW_TOP}));
}

/**
 * Given two rows 100 pixels wide on the same place, one drawn whole and the
 * other drawn at column 80 alone, in its second word
 * When they are compared
 * Then they touch: the rows are read word after word, not only the first
 */
TEST(MaskOverlap, WideMasksTouchPastTheirFirstWord) {
  const PixelMask whole = fullMask(WIDE_SIDE, 1);
  const PixelMask single = rowDrawnAt(WIDE_SIDE, PIXEL_IN_THE_SECOND_WORD);

  EXPECT_TRUE(masksOverlap(PlacedMask{.mask = &whole, .left = 0, .top = 0},
                           PlacedMask{.mask = &single, .left = 0, .top = 0}));
}

/**
 * Given a mask laid at 0, 0 with its top-left pixel alone drawn
 * When an area over that pixel is tested against it
 * Then they touch
 */
TEST(MaskOverlap, AnAreaOverADrawnPixelTouchesIt) {
  const PixelMask mask = maskFromRows(TOP_LEFT_DRAWN_ROWS);

  EXPECT_TRUE(maskOverlapsArea(PlacedMask{.mask = &mask, .left = 0, .top = 0},
                               TOP_LEFT_CELL));
}

/**
 * Given a mask laid at 0, 0 with its top-left pixel alone drawn
 * When an area over every other pixel of it is tested against it
 * Then they do not touch: the area covers clear pixels only
 */
TEST(MaskOverlap, AnAreaOverClearPixelsOnlyDoesNotTouch) {
  const PixelMask mask = maskFromRows(TOP_LEFT_DRAWN_ROWS);

  EXPECT_FALSE(maskOverlapsArea(PlacedMask{.mask = &mask, .left = 0, .top = 0},
                                ALL_BUT_THE_FIRST_COLUMN));
}

/**
 * Given a full mask laid at 0, 0
 * When the column just past its right edge is tested against it
 * Then they do not touch
 */
TEST(MaskOverlap, AnAreaRightBesideTheMaskDoesNotTouch) {
  const PixelMask mask = fullMask(SMALL_WIDTH, SMALL_HEIGHT);

  EXPECT_FALSE(maskOverlapsArea(PlacedMask{.mask = &mask, .left = 0, .top = 0},
                                COLUMN_PAST_THE_RIGHT_EDGE));
}

/**
 * Given a full mask laid at 0, 0
 * When the row just above its top edge is tested against it
 * Then they do not touch
 */
TEST(MaskOverlap, AnAreaAboveTheMaskDoesNotTouch) {
  const PixelMask mask = fullMask(SMALL_WIDTH, SMALL_HEIGHT);

  EXPECT_FALSE(maskOverlapsArea(PlacedMask{.mask = &mask, .left = 0, .top = 0},
                                ROW_ABOVE_THE_TOP_EDGE));
}

/**
 * Given a full mask laid at 0, 0
 * When an area that starts left of it and reaches its first column is tested
 * Then they touch
 */
TEST(MaskOverlap, AnAreaReachingTheFirstColumnTouches) {
  const PixelMask mask = fullMask(SMALL_WIDTH, SMALL_HEIGHT);

  EXPECT_TRUE(maskOverlapsArea(PlacedMask{.mask = &mask, .left = 0, .top = 0},
                               AREA_REACHING_THE_FIRST_COLUMN));
}

/**
 * Given a full mask laid at 0, 0
 * When an area of no column is tested against it
 * Then they do not touch
 */
TEST(MaskOverlap, AnAreaOfNoColumnTouchesNothing) {
  const PixelMask mask = fullMask(SMALL_WIDTH, SMALL_HEIGHT);

  EXPECT_FALSE(maskOverlapsArea(PlacedMask{.mask = &mask, .left = 0, .top = 0},
                                AREA_OF_NO_COLUMN));
}

/**
 * Given a row 100 pixels wide drawn at column 80 alone, in its second word
 * When the cells of the whole row are tested against it
 * Then they touch: the row is read word after word, not only the first
 */
TEST(MaskOverlap, AnAreaReachesAPixelPastTheFirstWord) {
  const PixelMask single = rowDrawnAt(WIDE_SIDE, PIXEL_IN_THE_SECOND_WORD);

  EXPECT_TRUE(maskOverlapsArea(PlacedMask{.mask = &single, .left = 0, .top = 0},
                               WHOLE_WIDE_ROW));
}

/**
 * Given a mask with its top-left pixel alone drawn, laid away from the origin
 * When the cell under that pixel, on the grid, is tested against it
 * Then they touch: the area is read on the grid, not on the mask
 */
TEST(MaskOverlap, AnAreaIsReadOnTheGridTheMaskLiesOn) {
  const PixelMask mask = maskFromRows(TOP_LEFT_DRAWN_ROWS);

  EXPECT_TRUE(maskOverlapsArea(
      PlacedMask{.mask = &mask, .left = MASK_AWAY_LEFT, .top = MASK_AWAY_TOP},
      CELL_UNDER_THE_MOVED_TOP_LEFT));
}
