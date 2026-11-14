#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetHitboxPartsTestConstants.hpp"
#include "EntitySheetMaskFixtures.hpp"
#include "EntitySheetMaskTestConstants.hpp"
#include "EntitySheetReaderFixture.hpp"
#include "EntitySheetReaderTestConstants.hpp"
#include "MaskSheet.hpp"

using rtype::engine::DataFileValueOutOfRangeException;
using rtype::game::EntitySheet;
using rtype::game::MaskSheet;
using rtype::game::testing::APPEARANCE;
using rtype::game::testing::BYDO_HITBOX;
using rtype::game::testing::CORNER_OF_THE_CROSS;
using rtype::game::testing::CROSS_SIDE;
using rtype::game::testing::ENEMY_GAMEPLAY;
using rtype::game::testing::EntitySheetReaderFixture;
using rtype::game::testing::FIRST_LINE_OF_THE_MASK;
using rtype::game::testing::HITBOX_OF_TWO_BOXES;
using rtype::game::testing::LEFT_HEADING_TABLE;
using rtype::game::testing::MASK_AND_PARTS_RULE_TEXT;
using rtype::game::testing::MASK_COLOUR_DOTTED_KEY;
using rtype::game::testing::MASK_DOTTED_KEY;
using rtype::game::testing::MASK_DRAWN_RULE_TEXT;
using rtype::game::testing::MASK_FILE_DOTTED_KEY;
using rtype::game::testing::MASK_FILE_RULE_TEXT;
using rtype::game::testing::MASK_OF_A_CROSS;
using rtype::game::testing::MASK_OF_A_CROSS_WITH_CARRIAGE_RETURNS;
using rtype::game::testing::MASK_OF_A_SHORT_SECOND_ROW;
using rtype::game::testing::MASK_OF_ANOTHER_CHARACTER;
using rtype::game::testing::MASK_OF_NO_ROW;
using rtype::game::testing::MASK_OF_ONE_PIXEL;
using rtype::game::testing::MASK_OF_UNKNOWN_CHARACTERS_ONLY;
using rtype::game::testing::MASK_PIXEL_RULE_TEXT;
using rtype::game::testing::MASK_ROW_WIDTH_RULE_TEXT;
using rtype::game::testing::MASK_SIDE_RULE_TEXT;
using rtype::game::testing::MASK_TABLE;
using rtype::game::testing::MASK_TABLE_OF_AN_UNKNOWN_KEY;
using rtype::game::testing::MASK_TABLE_OF_ANOTHER_SHEET;
using rtype::game::testing::MASK_TABLE_OF_TOO_MUCH_EROSION;
using rtype::game::testing::MASK_TABLE_PEELED_ONCE;
using rtype::game::testing::MASK_THAT_DRAWS_NOTHING;
using rtype::game::testing::maskOfOneRow;
using rtype::game::testing::ONE_PIXEL_PEELED;
using rtype::game::testing::READ_SHEET_SOURCE;
using rtype::game::testing::RIGHT_ARM_OF_THE_CROSS;
using rtype::game::testing::ROW_PAST_THE_WIDEST;
using rtype::game::testing::SECOND_LINE_OF_THE_MASK;
using rtype::game::testing::STEADY_MOTION;
using rtype::game::testing::TOP_OF_THE_CROSS;
using rtype::game::testing::WIDEST_MASK_ROW;
using ::testing::HasSubstr;

/**
 * Given the sheet of an enemy naming its mask file, which draws a cross of 3
 * by 3 pixels
 * When its gameplay is read
 * Then the mask is 3 by 3, drawn where a line writes # and clear where it
 * writes .
 */
TEST_F(EntitySheetReaderFixture, ReadsTheMaskFileRowByRow) {
  const EntitySheet sheet =
      readWithMask({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                    LEFT_HEADING_TABLE, MASK_TABLE, APPEARANCE},
                   MASK_OF_A_CROSS);

  ASSERT_TRUE(sheet.mask.has_value());
  const MaskSheet mask = sheet.mask.value_or(MaskSheet{});
  ASSERT_NE(mask.pixels, nullptr);
  EXPECT_EQ(mask.pixels->width(), CROSS_SIDE);
  EXPECT_EQ(mask.pixels->height(), CROSS_SIDE);
  EXPECT_TRUE(mask.pixels->isDrawn(TOP_OF_THE_CROSS));
  EXPECT_FALSE(mask.pixels->isDrawn(CORNER_OF_THE_CROSS));
  EXPECT_TRUE(mask.pixels->isDrawn(RIGHT_ARM_OF_THE_CROSS));
}

/**
 * Given the same cross written with Windows line endings
 * When the gameplay of the sheet is read
 * Then the mask is the same 3 by 3 cross: a carriage return is no pixel
 */
TEST_F(EntitySheetReaderFixture, ReadsAMaskFileWrittenWithCarriageReturns) {
  const EntitySheet sheet =
      readWithMask({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                    LEFT_HEADING_TABLE, MASK_TABLE, APPEARANCE},
                   MASK_OF_A_CROSS_WITH_CARRIAGE_RETURNS);

  ASSERT_TRUE(sheet.mask.has_value());
  const MaskSheet mask = sheet.mask.value_or(MaskSheet{});
  ASSERT_NE(mask.pixels, nullptr);
  EXPECT_EQ(mask.pixels->width(), CROSS_SIDE);
  EXPECT_TRUE(mask.pixels->isDrawn(RIGHT_ARM_OF_THE_CROSS));
}

/**
 * Given the sheet of an enemy without mask
 * When its gameplay is read
 * Then it has none: it collides with its hitbox alone
 */
TEST_F(EntitySheetReaderFixture, ASheetWithoutMaskHasNone) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_FALSE(sheet.mask.has_value());
}

/**
 * Given the sheet of an enemy whose mask gives no erosion
 * When its gameplay is read
 * Then its erosion is 0: the mask is the sprite, unpeeled
 */
TEST_F(EntitySheetReaderFixture, TheErosionOfAMaskIsZeroWhenLeftOut) {
  const EntitySheet sheet =
      readWithMask({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                    LEFT_HEADING_TABLE, MASK_TABLE, APPEARANCE},
                   MASK_OF_A_CROSS);

  ASSERT_TRUE(sheet.mask.has_value());
  EXPECT_EQ(sheet.mask.value_or(MaskSheet{}).erosion, 0);
}

/**
 * Given the sheet of an enemy whose mask was peeled once
 * When its gameplay is read
 * Then its erosion is 1
 */
TEST_F(EntitySheetReaderFixture, ReadsTheErosionOfAMask) {
  const EntitySheet sheet =
      readWithMask({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                    LEFT_HEADING_TABLE, MASK_TABLE_PEELED_ONCE, APPEARANCE},
                   MASK_OF_A_CROSS);

  ASSERT_TRUE(sheet.mask.has_value());
  EXPECT_EQ(sheet.mask.value_or(MaskSheet{}).erosion, ONE_PIXEL_PEELED);
}

/**
 * Given the sheet of an enemy naming a mask file, with no such file next to it
 * When its gameplay is read
 * Then it is refused at the key that names the file
 */
TEST_F(EntitySheetReaderFixture, RefusesAMaskWhoseFileIsMissing) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 MASK_TABLE, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(READ_SHEET_SOURCE));
  EXPECT_THAT(message, HasSubstr(MASK_FILE_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(MASK_FILE_RULE_TEXT));
}

/**
 * Given the sheet of an enemy that names the mask file of another sheet, and
 * a mask file of its own next to it
 * When its gameplay is read
 * Then it is refused at the key that names the file: a sheet reads its own
 * mask file only
 */
TEST_F(EntitySheetReaderFixture, RefusesAMaskNamingTheFileOfAnotherSheet) {
  const std::string message = maskRefusalOf(
      {ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
       MASK_TABLE_OF_ANOTHER_SHEET, APPEARANCE},
      MASK_OF_A_CROSS);

  EXPECT_THAT(message, HasSubstr(MASK_FILE_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(MASK_FILE_RULE_TEXT));
}

/**
 * Given the sheet of an enemy whose mask file has a second line shorter than
 * the first
 * When its gameplay is read
 * Then it is refused at that line of the mask file: a mask is a rectangle
 */
TEST_F(EntitySheetReaderFixture, RefusesAMaskRowOfAnotherWidth) {
  const std::string message =
      maskRefusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                     LEFT_HEADING_TABLE, MASK_TABLE, APPEARANCE},
                    MASK_OF_A_SHORT_SECOND_ROW);

  EXPECT_THAT(message, HasSubstr(SECOND_LINE_OF_THE_MASK));
  EXPECT_THAT(message, HasSubstr(MASK_ROW_WIDTH_RULE_TEXT));
}

/**
 * Given the sheet of an enemy whose mask file writes a pixel with an x
 * When its gameplay is read
 * Then it is refused at that line of the mask file: a pixel is # or .
 */
TEST_F(EntitySheetReaderFixture, RefusesAnotherCharacterInAMask) {
  const std::string message =
      maskRefusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                     LEFT_HEADING_TABLE, MASK_TABLE, APPEARANCE},
                    MASK_OF_ANOTHER_CHARACTER);

  EXPECT_THAT(message, HasSubstr(FIRST_LINE_OF_THE_MASK));
  EXPECT_THAT(message, HasSubstr(MASK_PIXEL_RULE_TEXT));
}

/**
 * Given the sheet of an enemy whose mask file holds one line of x, so no #
 * When its gameplay is read
 * Then it is refused for its character, not for drawing nothing: a row is
 * checked before what it draws is counted
 */
TEST_F(EntitySheetReaderFixture, RefusesAnUnknownCharacterBeforeAnEmptyMask) {
  const std::string message =
      maskRefusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                     LEFT_HEADING_TABLE, MASK_TABLE, APPEARANCE},
                    MASK_OF_UNKNOWN_CHARACTERS_ONLY);

  EXPECT_THAT(message, HasSubstr(MASK_PIXEL_RULE_TEXT));
}

/**
 * Given the sheet of an enemy whose mask file is empty
 * When its gameplay is read
 * Then it is refused at the first line of the mask file
 */
TEST_F(EntitySheetReaderFixture, RefusesAMaskFileWithoutRow) {
  const std::string message =
      maskRefusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                     LEFT_HEADING_TABLE, MASK_TABLE, APPEARANCE},
                    MASK_OF_NO_ROW);

  EXPECT_THAT(message, HasSubstr(FIRST_LINE_OF_THE_MASK));
  EXPECT_THAT(message, HasSubstr(MASK_SIDE_RULE_TEXT));
}

/**
 * Given the sheet of an enemy whose mask file is one row of 256 pixels, the
 * widest a mask may be
 * When its gameplay is read
 * Then the mask is 256 pixels wide
 */
TEST_F(EntitySheetReaderFixture, ReadsAMaskRowAsWideAsTheWidest) {
  const std::string mask = maskOfOneRow(WIDEST_MASK_ROW);
  const EntitySheet sheet =
      readWithMask({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                    LEFT_HEADING_TABLE, MASK_TABLE, APPEARANCE},
                   mask);

  ASSERT_TRUE(sheet.mask.has_value());
  const MaskSheet read = sheet.mask.value_or(MaskSheet{});
  ASSERT_NE(read.pixels, nullptr);
  EXPECT_EQ(read.pixels->width(), static_cast<int>(WIDEST_MASK_ROW));
}

/**
 * Given the sheet of an enemy whose mask file is one row of 257 pixels
 * When its gameplay is read
 * Then it is refused at the first line of the mask file: a mask is at most
 * 256 pixels wide
 */
TEST_F(EntitySheetReaderFixture, RefusesAMaskRowPastTheWidest) {
  const std::string mask = maskOfOneRow(ROW_PAST_THE_WIDEST);
  const std::string message =
      maskRefusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                     LEFT_HEADING_TABLE, MASK_TABLE, APPEARANCE},
                    mask);

  EXPECT_THAT(message, HasSubstr(FIRST_LINE_OF_THE_MASK));
  EXPECT_THAT(message, HasSubstr(MASK_SIDE_RULE_TEXT));
}

/**
 * Given the sheet of an enemy whose mask file draws no pixel
 * When its gameplay is read
 * Then it is refused at the first line of the mask file: such an enemy could
 * never be touched
 */
TEST_F(EntitySheetReaderFixture, RefusesAMaskThatDrawsNothing) {
  const std::string message =
      maskRefusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                     LEFT_HEADING_TABLE, MASK_TABLE, APPEARANCE},
                    MASK_THAT_DRAWS_NOTHING);

  EXPECT_THAT(message, HasSubstr(FIRST_LINE_OF_THE_MASK));
  EXPECT_THAT(message, HasSubstr(MASK_DRAWN_RULE_TEXT));
}

/**
 * Given the sheet of an enemy that gives a mask and lists two hitbox boxes
 * When its gameplay is read
 * Then it is refused at its mask: what it is touched on would be unclear
 */
TEST_F(EntitySheetReaderFixture, RefusesAMaskBesideSeveralBoxes) {
  const std::string message =
      maskRefusalOf({ENEMY_GAMEPLAY, HITBOX_OF_TWO_BOXES, STEADY_MOTION,
                     LEFT_HEADING_TABLE, MASK_TABLE, APPEARANCE},
                    MASK_OF_ONE_PIXEL);

  EXPECT_THAT(message, HasSubstr(MASK_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(MASK_AND_PARTS_RULE_TEXT));
}

/**
 * Given the sheet of an enemy whose mask has a key no mask has
 * When its gameplay is read
 * Then it is refused at that key
 */
TEST_F(EntitySheetReaderFixture, RefusesAnUnknownKeyOfAMask) {
  const std::string message = maskRefusalOf(
      {ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
       MASK_TABLE_OF_AN_UNKNOWN_KEY, APPEARANCE},
      MASK_OF_ONE_PIXEL);

  EXPECT_THAT(message, HasSubstr(MASK_COLOUR_DOTTED_KEY));
}

/**
 * Given the sheet of an enemy whose mask was peeled 5 times
 * When its gameplay is read
 * Then it is refused: an erosion is 0 to 4
 */
TEST_F(EntitySheetReaderFixture, RefusesAnErosionOutOfRange) {
  EXPECT_THROW(
      static_cast<void>(readWithMask(
          {ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
           MASK_TABLE_OF_TOO_MUCH_EROSION, APPEARANCE},
          MASK_OF_ONE_PIXEL)),
      DataFileValueOutOfRangeException);
}
