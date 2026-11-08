#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <cstddef>
#include "SheetPixels.hpp"
#include "ShipLookSpriteConstants.hpp"
#include "SpriteFrame.hpp"

using rtype::client::SHIP_LOOK_CELLS;
using rtype::client::SHIP_LOOK_SHEET_ID;
using rtype::client::SHIP_LOOK_SPRITES;
using rtype::game::SpriteFrame;

/**
 * Given the sheet of the items shipped in assets/
 * When the drawing of each item is placed on it, from the corner of its cell
 * and at the size of its sprite
 * Then it lies inside the sheet, shows a drawing, and cuts none
 */
TEST(ShipLookSpriteConstants, EveryItemHoldsAWholeDrawing) {
  const sf::Image sheet = shippedSheet(SHIP_LOOK_SHEET_ID);
  for (std::size_t item = 0; item < SHIP_LOOK_SPRITES.size(); ++item) {
    SCOPED_TRACE(item);
    const SpriteFrame frame{.left = SHIP_LOOK_CELLS.at(item).left,
                            .top = SHIP_LOOK_CELLS.at(item).top,
                            .width = SHIP_LOOK_SPRITES.at(item).size.x,
                            .height = SHIP_LOOK_SPRITES.at(item).size.y};
    EXPECT_TRUE(liesInside(frame, sheet.getSize()));
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
  }
}
