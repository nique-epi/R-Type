#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "CasinoSpriteConstants.hpp"
#include "CasinoSpriteConstantsTestConstants.hpp"
#include "InterfaceConstants.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::BUTLER_FRAME_SIZE;
using rtype::client::BUTLER_FRAMES_PER_POSE;
using rtype::client::BUTLER_SHEET_ID;
using rtype::client::CASINO_DOORS_AJAR_ID;
using rtype::client::CASINO_DOORS_CLOSED_ID;
using rtype::client::CASINO_DOORS_OPEN_ID;
using rtype::client::CHIP_SELECTOR_FRAME_SIZE;
using rtype::client::CHIP_SELECTOR_SHEET_ID;
using rtype::client::CHIP_SELECTOR_STRIDE;
using rtype::client::INTERFACE_AREA;
using rtype::client::testing::BUTLER_POSE_COUNT;
using rtype::client::testing::SELECTOR_CHIP_COUNT;
using rtype::game::SpriteFrame;

/**
 * Given the three pictures of the front of the casino shipped in assets/
 * When they are read
 * Then each is as large as the interface picture, which it fills
 */
TEST(CasinoSpriteConstants, TheFrontOfTheCasinoFillsTheInterface) {
  for (const std::string_view front :
       {CASINO_DOORS_CLOSED_ID, CASINO_DOORS_AJAR_ID, CASINO_DOORS_OPEN_ID}) {
    SCOPED_TRACE(front);
    EXPECT_EQ(sf::Vector2i(shippedSheet(front).getSize()), INTERFACE_AREA.size);
  }
}

/**
 * Given the sheet of the butler shipped in assets/
 * When it is read
 * Then it holds BUTLER_POSE_COUNT rows of BUTLER_FRAMES_PER_POSE frames, one
 * row per pose, and no more
 */
TEST(CasinoSpriteConstants, TheButlerSheetHoldsThreePosesOfFourFrames) {
  const sf::Vector2i expected(BUTLER_FRAME_SIZE.x * BUTLER_FRAMES_PER_POSE,
                              BUTLER_FRAME_SIZE.y * BUTLER_POSE_COUNT);

  EXPECT_EQ(sf::Vector2i(shippedSheet(BUTLER_SHEET_ID).getSize()), expected);
}

/**
 * Given the sheet of the chips of the selector shipped in assets/
 * When each of its five chips is placed on it, CHIP_SELECTOR_STRIDE apart
 * Then each lies inside the sheet, shows a drawing, and cuts none
 */
TEST(CasinoSpriteConstants, EachChipOfTheSelectorHoldsAWholeDrawing) {
  const sf::Image sheet = shippedSheet(CHIP_SELECTOR_SHEET_ID);
  for (int chip = 0; chip < SELECTOR_CHIP_COUNT; ++chip) {
    SCOPED_TRACE(chip);
    const SpriteFrame frame{.left = chip * CHIP_SELECTOR_STRIDE,
                            .top = 0,
                            .width = CHIP_SELECTOR_FRAME_SIZE.x,
                            .height = CHIP_SELECTOR_FRAME_SIZE.y};
    EXPECT_TRUE(liesInside(frame, sheet.getSize()));
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
  }
}
