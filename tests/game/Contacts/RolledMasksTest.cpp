#include <gtest/gtest.h>
#include <cstddef>
#include "MaskPixel.hpp"
#include "PixelMask.hpp"
#include "RolledMasks.hpp"
#include "RolledMasksFixtures.hpp"
#include "RolledMasksTestConstants.hpp"

using rtype::engine::MaskPixel;
using rtype::engine::PixelMask;
using rtype::game::RolledMasks;
using rtype::game::testing::A_FIFTH_INTO_THE_ROLL;
using rtype::game::testing::DIAGONAL_SIDE;
using rtype::game::testing::diagonalMask;
using rtype::game::testing::FIRST_ROW_SHOWN_TWO_FIFTHS_IN;
using rtype::game::testing::LAST_DIAGONAL_ROW;
using rtype::game::testing::LAST_ROW_SHOWN_TWO_FIFTHS_IN;
using rtype::game::testing::ROWS_SHOWN_A_FIFTH_IN;
using rtype::game::testing::ROWS_SHOWN_TWO_FIFTHS_IN;
using rtype::game::testing::START_OF_THE_ROLL;
using rtype::game::testing::TWO_FIFTHS_INTO_THE_ROLL;

/**
 * Given the masks a diagonal mask of 16 rows shows through a roll
 * When the mask shown at the start of the roll is taken
 * Then it is the upright mask: 16 rows, each its own
 */
TEST(RolledMasks, TheStartOfTheRollShowsTheUprightMask) {
  const RolledMasks rolled{diagonalMask(DIAGONAL_SIDE)};

  const PixelMask& shown = rolled.shownAt(START_OF_THE_ROLL);

  ASSERT_EQ(shown.height(), DIAGONAL_SIDE);
  EXPECT_TRUE(shown.isDrawn(MaskPixel{.x = 0, .y = 0}));
  EXPECT_TRUE(
      shown.isDrawn(MaskPixel{.x = LAST_DIAGONAL_ROW, .y = LAST_DIAGONAL_ROW}));
}

/**
 * Given the masks a diagonal mask of 16 rows shows through a roll
 * When the mask shown a fifth into the roll is taken
 * Then it has the 5 rows the client draws, each copying the row of the
 * upright mask drawn there
 */
TEST(RolledMasks, AFifthIntoTheRollShowsTheRowsTheClientDraws) {
  const RolledMasks rolled{diagonalMask(DIAGONAL_SIDE)};

  const PixelMask& shown = rolled.shownAt(A_FIFTH_INTO_THE_ROLL);

  ASSERT_EQ(static_cast<std::size_t>(shown.height()),
            ROWS_SHOWN_A_FIFTH_IN.size());
  for (std::size_t row = 0; row < ROWS_SHOWN_A_FIFTH_IN.size(); ++row) {
    SCOPED_TRACE(row);
    EXPECT_TRUE(shown.isDrawn(MaskPixel{.x = ROWS_SHOWN_A_FIFTH_IN.at(row),
                                        .y = static_cast<int>(row)}));
  }
}

/**
 * Given the masks a diagonal mask of 16 rows shows through a roll
 * When the mask shown two fifths into the roll, on its belly, is taken
 * Then it has 13 rows, upside down: its first copies the last row of the
 * upright mask, its last the first
 */
TEST(RolledMasks, TheBellyShowsTheRowsUpsideDown) {
  const RolledMasks rolled{diagonalMask(DIAGONAL_SIDE)};

  const PixelMask& shown = rolled.shownAt(TWO_FIFTHS_INTO_THE_ROLL);

  ASSERT_EQ(shown.height(), ROWS_SHOWN_TWO_FIFTHS_IN);
  EXPECT_TRUE(
      shown.isDrawn(MaskPixel{.x = FIRST_ROW_SHOWN_TWO_FIFTHS_IN, .y = 0}));
  EXPECT_TRUE(shown.isDrawn(MaskPixel{.x = LAST_ROW_SHOWN_TWO_FIFTHS_IN,
                                      .y = ROWS_SHOWN_TWO_FIFTHS_IN - 1}));
}
