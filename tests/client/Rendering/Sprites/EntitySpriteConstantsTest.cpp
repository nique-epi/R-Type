#include <gtest/gtest.h>
#include "BackgroundConstants.hpp"
#include "EntitySpriteConstants.hpp"
#include "ShipControlConstants.hpp"

using rtype::client::BACKGROUND_PIXEL_SIZE;
using rtype::client::PLAYER_SHIP_FRAME;
using rtype::game::SHIP_HEIGHT;
using rtype::game::SHIP_WIDTH;

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
