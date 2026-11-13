#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "EntitySheet.hpp"
#include "EntitySheetBossTestConstants.hpp"
#include "EntitySheetHitboxPartsTestConstants.hpp"
#include "EntitySheetReaderFixture.hpp"
#include "EntitySheetReaderTestConstants.hpp"

using rtype::game::EntitySheet;
using rtype::game::testing::APPEARANCE;
using rtype::game::testing::ATTACK_OF_THE_FIRST_BOX_DOTTED_KEY;
using rtype::game::testing::BOX_AROUND_TWO_BOXES;
using rtype::game::testing::BYDO_HITBOX;
using rtype::game::testing::ENEMY_GAMEPLAY;
using rtype::game::testing::ENEMY_MISSILE_GAMEPLAY;
using rtype::game::testing::EntitySheetReaderFixture;
using rtype::game::testing::HEAD_OF_TWO_BOXES;
using rtype::game::testing::HITBOX_BOX_DOTTED_KEY;
using rtype::game::testing::HITBOX_BOXES_RULE_TEXT;
using rtype::game::testing::HITBOX_DOTTED_KEY;
using rtype::game::testing::HITBOX_OF_A_BOX_WITH_AN_ATTACK;
using rtype::game::testing::HITBOX_OF_A_SIZE_AND_A_BOX;
using rtype::game::testing::HITBOX_OF_NO_BOX;
using rtype::game::testing::HITBOX_OF_TWO_BOXES;
using rtype::game::testing::HITBOX_PARTS_SIDE_RULE_TEXT;
using rtype::game::testing::HITBOX_SHAPE_RULE_TEXT;
using rtype::game::testing::HITBOX_WIDTH_DOTTED_KEY;
using rtype::game::testing::LEFT_HEADING_TABLE;
using rtype::game::testing::STEADY_MOTION;
using rtype::game::testing::TRUNK_OF_TWO_BOXES;
using rtype::game::testing::TWO_BOXES;
using ::testing::HasSubstr;

/**
 * Given the sheet of an enemy whose hitbox lists a head and a trunk
 * When its gameplay is read
 * Then it keeps those two boxes, in the order of the file, each with its size
 * and its offset, 0 where the file gives none
 */
TEST_F(EntitySheetReaderFixture, ReadsTheBoxesAHitboxLists) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, HITBOX_OF_TWO_BOXES, STEADY_MOTION,
            LEFT_HEADING_TABLE, APPEARANCE});

  ASSERT_EQ(sheet.hitboxParts.size(), TWO_BOXES);
  EXPECT_EQ(sheet.hitboxParts.front(), HEAD_OF_TWO_BOXES);
  EXPECT_EQ(sheet.hitboxParts.back(), TRUNK_OF_TWO_BOXES);
}

/**
 * Given the same sheet, whose boxes run from x -100 to 80 and from y -20 to
 * 80
 * When its gameplay is read
 * Then its hitbox is the rectangle that encloses them: 180 by 100, 10 units
 * left of its Position and 30 below
 */
TEST_F(EntitySheetReaderFixture, TheHitboxOfSeveralBoxesEnclosesThem) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, HITBOX_OF_TWO_BOXES, STEADY_MOTION,
            LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_EQ(sheet.hitbox, BOX_AROUND_TWO_BOXES);
}

/**
 * Given the sheet of an enemy whose hitbox gives its width and its height
 * When its gameplay is read
 * Then it lists no box: it collides with its hitbox alone
 */
TEST_F(EntitySheetReaderFixture, AHitboxOfOneBoxListsNone) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_TRUE(sheet.hitboxParts.empty());
}

/**
 * Given the sheet of an enemy whose hitbox gives a width and lists a box
 * When its gameplay is read
 * Then it is refused at that width: a hitbox is one box or a list
 */
TEST_F(EntitySheetReaderFixture, RefusesAHitboxOfASizeAndAList) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, HITBOX_OF_A_SIZE_AND_A_BOX, STEADY_MOTION,
                 LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(HITBOX_WIDTH_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(HITBOX_SHAPE_RULE_TEXT));
}

/**
 * Given the sheet of an enemy whose hitbox lists no box at all
 * When its gameplay is read
 * Then it is refused at its list: a body collides with at least one box
 */
TEST_F(EntitySheetReaderFixture, RefusesAHitboxThatListsNoBox) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, HITBOX_OF_NO_BOX, STEADY_MOTION,
                 LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(HITBOX_BOX_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(HITBOX_BOXES_RULE_TEXT));
}

/**
 * Given the sheet of an enemy whose listed box names an attack, a key of a
 * box of a hurt zone
 * When its gameplay is read
 * Then it is refused at that key: a box of a hitbox has a size and an offset
 * only
 */
TEST_F(EntitySheetReaderFixture, RefusesAnUnknownKeyInAListedBox) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, HITBOX_OF_A_BOX_WITH_AN_ATTACK, STEADY_MOTION,
                 LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(ATTACK_OF_THE_FIRST_BOX_DOTTED_KEY));
}

/**
 * Given the sheet of a missile of an enemy whose hitbox lists two boxes
 * When its gameplay is read
 * Then it is refused at its hitbox: only an enemy collides with several boxes
 */
TEST_F(EntitySheetReaderFixture, RefusesSeveralBoxesOnAMissile) {
  const std::string message =
      refusalOf({ENEMY_MISSILE_GAMEPLAY, HITBOX_OF_TWO_BOXES, STEADY_MOTION,
                 LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(HITBOX_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(HITBOX_PARTS_SIDE_RULE_TEXT));
}
