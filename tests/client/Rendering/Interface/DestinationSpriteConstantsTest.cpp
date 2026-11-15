#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include "DestinationSpriteConstants.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::DESTINATION_BASKET_FRAME;
using rtype::client::DESTINATION_HANGAR_FRAME;
using rtype::client::DESTINATION_PLANET_FRAME;
using rtype::client::DESTINATION_SHEET_ID;
using rtype::client::DESTINATION_WHEEL_FRAME;
using rtype::game::SpriteFrame;

/**
 * Given the sheet of the destination screen shipped in assets/
 * When the planet of SERVER, the basket of SHOP, the hangar of HANGAR and the
 * wheel of CASINO are placed on it
 * Then each lies inside the sheet, shows a drawing, and cuts none
 */
TEST(DestinationSpriteConstants, EachSpriteHoldsAWholeDrawing) {
  const sf::Image sheet = shippedSheet(DESTINATION_SHEET_ID);
  for (const SpriteFrame frame :
       {DESTINATION_PLANET_FRAME, DESTINATION_BASKET_FRAME,
        DESTINATION_HANGAR_FRAME, DESTINATION_WHEEL_FRAME}) {
    SCOPED_TRACE(frame.left);
    EXPECT_TRUE(liesInside(frame, sheet.getSize()));
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
  }
}
