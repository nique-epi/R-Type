#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <cstddef>
#include <string_view>
#include "BackgroundConstants.hpp"
#include "CollisionBox.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "EntityTypesTestConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "PlayerColors.hpp"
#include "SheetPixels.hpp"
#include "ShipControlConstants.hpp"
#include "ShippedEntitySheets.hpp"
#include "SpriteFrame.hpp"

using rtype::client::BACKGROUND_PIXEL_SIZE;
using rtype::client::BONUS_BYDO_FRAMES;
using rtype::client::BYDO_FRAME;
using rtype::client::ENEMY_MISSILE_FRAME;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::inPlayerColorRow;
using rtype::client::PLAYER_COLOR_COUNT;
using rtype::client::PLAYER_COLORS;
using rtype::client::PLAYER_MISSILE_FRAME;
using rtype::client::PLAYER_SHIP_FRAME;
using rtype::client::PlayerColors;
using rtype::game::CollisionBox;
using rtype::game::SHIP_HEIGHT;
using rtype::game::SHIP_WIDTH;
using rtype::game::SpriteFrame;
using rtype::game::testing::DRIFTER_SHEET;
using rtype::game::testing::ENEMY_MISSILE_SHEET;
using rtype::game::testing::PLAYER_MISSILE_SHEET;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::WEAVER_SHEET;

/**
 * Given the first frame of the player ship on the entity sheet
 * When its size is measured in logical units, each pixel of the sheet covering
 * BACKGROUND_PIXEL_SIZE
 * Then it is the size the server keeps whole inside the playfield
 */
TEST(EntitySpriteConstants, TheShipDrawnIsTheShipTheServerKeepsOnScreen) {
  EXPECT_FLOAT_EQ(
      static_cast<float>(PLAYER_SHIP_FRAME.width) * BACKGROUND_PIXEL_SIZE,
      SHIP_WIDTH);
  EXPECT_FLOAT_EQ(
      static_cast<float>(PLAYER_SHIP_FRAME.height) * BACKGROUND_PIXEL_SIZE,
      SHIP_HEIGHT);
}

/**
 * Given the first frame of a missile of a player on the entity sheet, and the
 * sheet of that missile
 * When the frame is measured in logical units, each pixel of the sheet
 * covering BACKGROUND_PIXEL_SIZE
 * Then it is the size of the box the server flies a missile with
 */
TEST(EntitySpriteConstants, TheMissileDrawnIsTheMissileTheServerFlies) {
  const CollisionBox box =
      shippedEntitySheets().named(PLAYER_MISSILE_SHEET).hitbox;

  EXPECT_FLOAT_EQ(
      static_cast<float>(PLAYER_MISSILE_FRAME.width) * BACKGROUND_PIXEL_SIZE,
      box.width);
  EXPECT_FLOAT_EQ(
      static_cast<float>(PLAYER_MISSILE_FRAME.height) * BACKGROUND_PIXEL_SIZE,
      box.height);
}

/**
 * Given the first frame of a Bydo on the entity sheet, and the sheets of the
 * Drifter and the Weaver
 * When the frame is measured in logical units, each pixel of the sheet
 * covering BACKGROUND_PIXEL_SIZE
 * Then it is the size of the box the server hits each of them with
 */
TEST(EntitySpriteConstants, TheBydoDrawnIsTheBydoTheServerHits) {
  for (const std::string_view enemy : {DRIFTER_SHEET, WEAVER_SHEET}) {
    const CollisionBox box = shippedEntitySheets().named(enemy).hitbox;

    EXPECT_FLOAT_EQ(
        static_cast<float>(BYDO_FRAME.width) * BACKGROUND_PIXEL_SIZE, box.width)
        << enemy;
    EXPECT_FLOAT_EQ(
        static_cast<float>(BYDO_FRAME.height) * BACKGROUND_PIXEL_SIZE,
        box.height)
        << enemy;
  }
}

/**
 * Given the first frame of a missile of an enemy on the entity sheet, and the
 * sheet of that missile
 * When the frame is measured in logical units, each pixel of the sheet
 * covering BACKGROUND_PIXEL_SIZE
 * Then it is the size of the box the server flies a missile of an enemy with
 */
TEST(EntitySpriteConstants, TheEnemyMissileDrawnIsTheMissileTheServerFlies) {
  const CollisionBox box =
      shippedEntitySheets().named(ENEMY_MISSILE_SHEET).hitbox;

  EXPECT_FLOAT_EQ(
      static_cast<float>(ENEMY_MISSILE_FRAME.width) * BACKGROUND_PIXEL_SIZE,
      box.width);
  EXPECT_FLOAT_EQ(
      static_cast<float>(ENEMY_MISSILE_FRAME.height) * BACKGROUND_PIXEL_SIZE,
      box.height);
}

/**
 * Given the entity sheet shipped in assets/
 * When the frames of the bonus Bydo are read in it
 * Then each lies inside the sheet
 */
TEST(EntitySpriteConstants, TheBonusBydoFramesLieInsideTheSheet) {
  const sf::Image sheet = shippedSheet(ENTITY_SHEET_ID);

  for (const SpriteFrame frame : BONUS_BYDO_FRAMES) {
    SCOPED_TRACE(frame.left);
    EXPECT_TRUE(liesInside(frame, sheet.getSize()));
  }
}

/**
 * Given the entity sheet shipped in assets/
 * When the frames of the bonus Bydo are read in it
 * Then each holds a whole drawing: a visible pixel, and none just around it
 */
TEST(EntitySpriteConstants, TheBonusBydoFramesHoldWholeDrawings) {
  const sf::Image sheet = shippedSheet(ENTITY_SHEET_ID);

  for (const SpriteFrame frame : BONUS_BYDO_FRAMES) {
    SCOPED_TRACE(frame.left);
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
  }
}

/**
 * Given the entity sheet shipped in assets/
 * When the first frame of the ship of each player color is read
 * Then it shows the dark, main and light shades of that color, which a
 * palette for color blindness swaps
 */
TEST(EntitySpriteConstants, EachShipRowShowsTheThreeShadesOfItsColor) {
  const sf::Image sheet = shippedSheet(ENTITY_SHEET_ID);

  for (int color = 0; color < PLAYER_COLOR_COUNT; ++color) {
    SCOPED_TRACE(color);
    const SpriteFrame frame = inPlayerColorRow(PLAYER_SHIP_FRAME, color);
    const PlayerColors& shades =
        PLAYER_COLORS.at(static_cast<std::size_t>(color));
    EXPECT_TRUE(showsColor(sheet, frame, shades.dark));
    EXPECT_TRUE(showsColor(sheet, frame, shades.ship));
    EXPECT_TRUE(showsColor(sheet, frame, shades.light));
  }
}
