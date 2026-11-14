#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include "DestinationSpriteConstants.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::DESTINATION_BASKET_FRAME;
using rtype::client::DESTINATION_PLANET_FRAME;
using rtype::client::DESTINATION_SHEET_ID;
using rtype::game::SpriteFrame;

/**
 * Given the sheet of the destination screen shipped in assets/
 * When the planet of SERVER and the basket of SHOP are placed on it
 * Then each lies inside the sheet, shows a drawing, and cuts none
 */
TEST(DestinationSpriteConstants, EachSpriteHoldsAWholeDrawing) {
  const sf::Image sheet = shippedSheet(DESTINATION_SHEET_ID);
  for (const SpriteFrame frame :
       {DESTINATION_PLANET_FRAME, DESTINATION_BASKET_FRAME}) {
    SCOPED_TRACE(frame.left);
    EXPECT_TRUE(liesInside(frame, sheet.getSize()));
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
  }
}
