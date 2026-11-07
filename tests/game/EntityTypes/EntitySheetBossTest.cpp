#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "EntitySheet.hpp"
#include "EntitySheetBossTestConstants.hpp"
#include "EntitySheetReaderFixture.hpp"
#include "EntitySheetReaderTestConstants.hpp"

using rtype::game::EntitySheet;
using rtype::game::testing::APPEARANCE;
using rtype::game::testing::BYDO_HITBOX;
using rtype::game::testing::ENEMY_GAMEPLAY;
using rtype::game::testing::EntitySheetReaderFixture;
using rtype::game::testing::HITBOX_DOTTED_KEY;
using rtype::game::testing::LEFT_HEADING_TABLE;
using rtype::game::testing::MOVED_HITBOX;
using rtype::game::testing::MOVED_HITBOX_OFFSET_X;
using rtype::game::testing::MOVED_HITBOX_OFFSET_Y;
using rtype::game::testing::MOVED_HITBOX_SIDE;
using rtype::game::testing::MOVED_SHIP_HITBOX;
using rtype::game::testing::SHIP_GAMEPLAY;
using rtype::game::testing::SHIP_HITBOX_RULE_TEXT;
using rtype::game::testing::SHIP_MOTION;
using rtype::game::testing::STEADY_MOTION;
using ::testing::HasSubstr;

/**
 * Given the sheet of an enemy whose hitbox is 72 by 72, 76 units left of its
 * Position and 52 above it
 * When its gameplay is read
 * Then its box keeps that size and that offset
 */
TEST_F(EntitySheetReaderFixture, ReadsAMovedHitbox) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, MOVED_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_FLOAT_EQ(sheet.hitbox.width, MOVED_HITBOX_SIDE);
  EXPECT_FLOAT_EQ(sheet.hitbox.height, MOVED_HITBOX_SIDE);
  EXPECT_FLOAT_EQ(sheet.hitbox.offsetX, MOVED_HITBOX_OFFSET_X);
  EXPECT_FLOAT_EQ(sheet.hitbox.offsetY, MOVED_HITBOX_OFFSET_Y);
}

/**
 * Given the sheet of an enemy whose hitbox gives no offset
 * When its gameplay is read
 * Then its box is centered on its Position
 */
TEST_F(EntitySheetReaderFixture, ReadsAHitboxWithoutOffsetAsCentered) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_FLOAT_EQ(sheet.hitbox.offsetX, 0.0F);
  EXPECT_FLOAT_EQ(sheet.hitbox.offsetY, 0.0F);
}

/**
 * Given the sheet of a ship whose box is the right size but moved
 * When its gameplay is read
 * Then it is refused at its hitbox: shipAfter() keeps a centered box on
 * screen
 */
TEST_F(EntitySheetReaderFixture, RefusesAShipWithAMovedHitbox) {
  const std::string message =
      refusalOf({SHIP_GAMEPLAY, MOVED_SHIP_HITBOX, SHIP_MOTION, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(HITBOX_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(SHIP_HITBOX_RULE_TEXT));
}
