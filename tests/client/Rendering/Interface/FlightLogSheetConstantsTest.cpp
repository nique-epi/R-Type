#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <cstddef>
#include "FlightLogSheetConstants.hpp"
#include "FlightLogSheetConstantsTestConstants.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::CROSS_FRAME;
using rtype::client::FLIGHT_LOG_SHEET_ID;
using rtype::client::testing::FLIGHT_LOG_FRAMES;
using rtype::game::SpriteFrame;

/**
 * Given the flight log sheet shipped in assets/
 * When each of its frames is placed on it
 * Then every frame lies inside the sheet
 */
TEST(FlightLogSheetConstants, EveryFrameLiesInsideTheSheet) {
  const sf::Image sheet = shippedSheet(FLIGHT_LOG_SHEET_ID);
  for (const SpriteFrame frame : FLIGHT_LOG_FRAMES) {
    SCOPED_TRACE(frame.left);
    EXPECT_TRUE(liesInside(frame, sheet.getSize()));
  }
}

/**
 * Given the flight log sheet shipped in assets/
 * When each of its frames is read
 * Then every frame shows a drawing, and none cuts one
 */
TEST(FlightLogSheetConstants, EveryFrameHoldsAWholeDrawing) {
  const sf::Image sheet = shippedSheet(FLIGHT_LOG_SHEET_ID);
  for (const SpriteFrame frame : FLIGHT_LOG_FRAMES) {
    SCOPED_TRACE(frame.left);
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
  }
}

/**
 * Given the frames of the flight log sheet
 * When any two are compared
 * Then they share no pixel
 */
TEST(FlightLogSheetConstants, NoTwoFramesOverlap) {
  for (std::size_t first = 0; first < FLIGHT_LOG_FRAMES.size(); ++first) {
    for (std::size_t second = first + 1; second < FLIGHT_LOG_FRAMES.size();
         ++second) {
      EXPECT_FALSE(
          overlap(FLIGHT_LOG_FRAMES[first], FLIGHT_LOG_FRAMES[second]));
    }
  }
}

/**
 * Given the cross of QUIT in the flight log sheet
 * When its pixels are read
 * Then every visible pixel is white, so a tint gives it its color
 */
TEST(FlightLogSheetConstants, TheCrossIsWhite) {
  const sf::Image sheet = shippedSheet(FLIGHT_LOG_SHEET_ID);
  for (int row = 0; row < CROSS_FRAME.height; ++row) {
    for (int column = 0; column < CROSS_FRAME.width; ++column) {
      const sf::Color pixel =
          sheet.getPixel({static_cast<unsigned int>(CROSS_FRAME.left + column),
                          static_cast<unsigned int>(CROSS_FRAME.top + row)});
      if (pixel.a > 0) {
        EXPECT_EQ(sf::Color(pixel.r, pixel.g, pixel.b), sf::Color::White);
      }
    }
  }
}
