#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include "SectorMapSpriteConstants.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::HANGAR_HOUSE_FRAME;
using rtype::client::LARGE_PLANET_FRAMES;
using rtype::client::MEDIUM_PLANET_FRAMES;
using rtype::client::OTHER_VERSION_PLANET_FRAMES;
using rtype::client::SECTOR_MAP_SHEET_ID;
using rtype::client::SLOW_PLANET_FRAMES;
using rtype::client::SMALL_PLANET_FRAMES;
using rtype::game::SpriteFrame;

/**
 * Given the sheet of the map of the servers shipped in assets/
 * When each frame of each planet is placed on it
 * Then it lies inside the sheet, shows a drawing, and cuts none
 */
TEST(SectorMapSpriteConstants, EveryPlanetFrameHoldsAWholeDrawing) {
  const sf::Image sheet = shippedSheet(SECTOR_MAP_SHEET_ID);
  for (const auto& row : {SMALL_PLANET_FRAMES, MEDIUM_PLANET_FRAMES,
                          LARGE_PLANET_FRAMES, SLOW_PLANET_FRAMES}) {
    for (const SpriteFrame frame : row) {
      SCOPED_TRACE(frame.top);
      SCOPED_TRACE(frame.left);
      EXPECT_TRUE(liesInside(frame, sheet.getSize()));
      EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
      EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
    }
  }
}

/**
 * Given the sheet of the map of the servers shipped in assets/
 * When the still planet of another version and the house of the hangar are
 * placed on it
 * Then each lies inside the sheet, shows a drawing, and cuts none
 */
TEST(SectorMapSpriteConstants, TheStillGlyphsHoldWholeDrawings) {
  const sf::Image sheet = shippedSheet(SECTOR_MAP_SHEET_ID);
  for (const SpriteFrame frame :
       {OTHER_VERSION_PLANET_FRAMES.front(), HANGAR_HOUSE_FRAME}) {
    SCOPED_TRACE(frame.top);
    EXPECT_TRUE(liesInside(frame, sheet.getSize()));
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
  }
}
