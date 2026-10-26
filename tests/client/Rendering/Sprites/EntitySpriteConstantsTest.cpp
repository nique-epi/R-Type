#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include "BackgroundConstants.hpp"
#include "EntitySizeConstants.hpp"
#include "EntitySpriteConstants.hpp"
#include "SheetPixels.hpp"
#include "ShipControlConstants.hpp"
#include "SpriteFrame.hpp"

using rtype::client::BACKGROUND_PIXEL_SIZE;
using rtype::client::BONUS_BYDO_FRAMES;
using rtype::client::BYDO_FRAME;
using rtype::client::ENEMY_MISSILE_FRAME;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::PLAYER_MISSILE_FRAME;
using rtype::client::PLAYER_SHIP_FRAME;
using rtype::game::BYDO_HEIGHT;
using rtype::game::BYDO_WIDTH;
using rtype::game::ENEMY_MISSILE_HEIGHT;
using rtype::game::ENEMY_MISSILE_WIDTH;
using rtype::game::PLAYER_MISSILE_HEIGHT;
using rtype::game::PLAYER_MISSILE_WIDTH;
using rtype::game::SHIP_HEIGHT;
using rtype::game::SHIP_WIDTH;
using rtype::game::SpriteFrame;

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
 * Given the first frame of a missile of a player on the entity sheet
 * When its size is measured in logical units, each pixel of the sheet covering
 * BACKGROUND_PIXEL_SIZE
 * Then it is the size of the box the server flies a missile with
 */
TEST(EntitySpriteConstants, TheMissileDrawnIsTheMissileTheServerFlies) {
  EXPECT_FLOAT_EQ(
      static_cast<float>(PLAYER_MISSILE_FRAME.width) * BACKGROUND_PIXEL_SIZE,
      PLAYER_MISSILE_WIDTH);
  EXPECT_FLOAT_EQ(
      static_cast<float>(PLAYER_MISSILE_FRAME.height) * BACKGROUND_PIXEL_SIZE,
      PLAYER_MISSILE_HEIGHT);
}

/**
 * Given the first frame of a Bydo on the entity sheet
 * When its size is measured in logical units, each pixel of the sheet covering
 * BACKGROUND_PIXEL_SIZE
 * Then it is the size of the box the server hits a Bydo with
 */
TEST(EntitySpriteConstants, TheBydoDrawnIsTheBydoTheServerHits) {
  EXPECT_FLOAT_EQ(static_cast<float>(BYDO_FRAME.width) * BACKGROUND_PIXEL_SIZE,
                  BYDO_WIDTH);
  EXPECT_FLOAT_EQ(static_cast<float>(BYDO_FRAME.height) * BACKGROUND_PIXEL_SIZE,
                  BYDO_HEIGHT);
}

/**
 * Given the first frame of a missile of an enemy on the entity sheet
 * When its size is measured in logical units, each pixel of the sheet covering
 * BACKGROUND_PIXEL_SIZE
 * Then it is the size of the box the server flies a missile of an enemy with
 */
TEST(EntitySpriteConstants, TheEnemyMissileDrawnIsTheMissileTheServerFlies) {
  EXPECT_FLOAT_EQ(
      static_cast<float>(ENEMY_MISSILE_FRAME.width) * BACKGROUND_PIXEL_SIZE,
      ENEMY_MISSILE_WIDTH);
  EXPECT_FLOAT_EQ(
      static_cast<float>(ENEMY_MISSILE_FRAME.height) * BACKGROUND_PIXEL_SIZE,
      ENEMY_MISSILE_HEIGHT);
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
