#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <cstddef>
#include "SheetPixels.hpp"
#include "ShipHullSpriteConstants.hpp"
#include "SpriteFrame.hpp"

using rtype::client::SHIP_HULL_ROWS;
using rtype::client::SHIP_HULL_SHEET_ID;
using rtype::game::SpriteFrame;

/**
 * Given the sheet of the hulls shipped in assets/
 * When each frame of each row is placed on it
 * Then it lies inside the sheet, shows a drawing, and cuts none
 */
TEST(ShipHullSpriteConstants, EveryHullFrameHoldsAWholeDrawing) {
  const sf::Image sheet = shippedSheet(SHIP_HULL_SHEET_ID);
  for (std::size_t row = 0; row < SHIP_HULL_ROWS.size(); ++row) {
    SCOPED_TRACE(row);
    for (const SpriteFrame& frame : SHIP_HULL_ROWS.at(row)) {
      EXPECT_TRUE(liesInside(frame, sheet.getSize()));
      EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
      EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
    }
  }
}
