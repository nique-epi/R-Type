#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <vector>
#include "HudSpriteConstants.hpp"
#include "HudSpriteConstantsFixtures.hpp"
#include "HudSpriteConstantsTestConstants.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::HUD_SHEET_ID;
using rtype::client::LIFE_ROW;
using rtype::client::MARKER_FRAMES;
using rtype::client::PLUG_ICON_ID;
using rtype::client::testing::PLUG_ICON_FILE_SIZE;
using rtype::game::SpriteFrame;

/**
 * Given the HUD sheet shipped in assets/
 * When each of its frames is placed on it
 * Then every frame lies inside the sheet
 */
TEST(HudSpriteConstants, EveryFrameLiesInsideTheHudSheet) {
  const sf::Image sheet = shippedSheet(HUD_SHEET_ID);

  for (const SpriteFrame frame : everyHudFrame()) {
    SCOPED_TRACE(frame.left);
    EXPECT_TRUE(liesInside(frame, sheet.getSize()));
  }
}

/**
 * Given the HUD sheet shipped in assets/
 * When each of its frames is read
 * Then every frame shows a drawing, and none cuts one
 */
TEST(HudSpriteConstants, EveryFrameHoldsAWholeDrawing) {
  const sf::Image sheet = shippedSheet(HUD_SHEET_ID);

  for (const SpriteFrame frame : everyHudFrame()) {
    SCOPED_TRACE(frame.left);
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
  }
}

/**
 * Given the frames of the HUD sheet
 * When any two are compared
 * Then they share no pixel
 */
TEST(HudSpriteConstants, NoTwoFramesOverlap) {
  const std::vector<SpriteFrame> frames = everyHudFrame();

  for (std::size_t first = 0; first < frames.size(); ++first) {
    for (std::size_t second = first + 1; second < frames.size(); ++second) {
      EXPECT_FALSE(overlap(frames[first], frames[second]));
    }
  }
}

/**
 * Given the plug icon shipped in assets/
 * When its pixels are read
 * Then it is a 64-pixel square whose every visible pixel is white, so a tint
 * gives it its color
 */
TEST(HudSpriteConstants, ThePlugIconIsWhite) {
  const sf::Image icon = shippedSheet(PLUG_ICON_ID);

  ASSERT_EQ(icon.getSize(), PLUG_ICON_FILE_SIZE);
  for (unsigned int row = 0; row < icon.getSize().y; ++row) {
    for (unsigned int column = 0; column < icon.getSize().x; ++column) {
      const sf::Color pixel = icon.getPixel({column, row});
      if (pixel.a > 0) {
        EXPECT_EQ(sf::Color(pixel.r, pixel.g, pixel.b), sf::Color::White);
      }
    }
  }
}
