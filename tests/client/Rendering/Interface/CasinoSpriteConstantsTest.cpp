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
using rtype::client::CASINO_DOORS_CLOSED_ID;
using rtype::client::CASINO_DOORS_OPEN_ID;
using rtype::client::CASHIER_ROOM_ID;
using rtype::client::CHIP_SELECTOR_FRAME_SIZE;
using rtype::client::CHIP_SELECTOR_SHEET_ID;
using rtype::client::CHIP_SELECTOR_STRIDE;
using rtype::client::HALL_BOXER_FRAME_SIZE;
using rtype::client::HALL_BOXER_FRAMES;
using rtype::client::HALL_BOXERS_SHEET_ID;
using rtype::client::HALL_PICTURE_ID;
using rtype::client::INTERFACE_AREA;
using rtype::client::TELLER_FRAME_SIZE;
using rtype::client::TELLER_FRAMES_PER_POSE;
using rtype::client::TELLER_SHEET_ID;
using rtype::client::testing::BUTLER_POSE_COUNT;
using rtype::client::testing::SELECTOR_CHIP_COUNT;
using rtype::client::testing::TELLER_POSE_COUNT;
using rtype::game::SpriteFrame;

/**
 * Given the pictures of the rooms of the casino shipped in assets/: its
 * front, doors closed and open, its hall and its cashier
 * When they are read
 * Then each is as large as the interface picture, which it fills
 */
TEST(CasinoSpriteConstants, EachRoomOfTheCasinoFillsTheInterface) {
  for (const std::string_view room :
       {CASINO_DOORS_CLOSED_ID, CASINO_DOORS_OPEN_ID, HALL_PICTURE_ID,
        CASHIER_ROOM_ID}) {
    SCOPED_TRACE(room);
    EXPECT_EQ(sf::Vector2i(shippedSheet(room).getSize()), INTERFACE_AREA.size);
  }
}

/**
 * Given the strip of the boxers of the hall shipped in assets/
 * When it is read
 * Then it holds HALL_BOXER_FRAMES frames side by side, and no more
 */
TEST(CasinoSpriteConstants, TheBoxersStripHoldsEightFramesInARow) {
  const sf::Vector2i expected(HALL_BOXER_FRAME_SIZE.x * HALL_BOXER_FRAMES,
                              HALL_BOXER_FRAME_SIZE.y);

  EXPECT_EQ(sf::Vector2i(shippedSheet(HALL_BOXERS_SHEET_ID).getSize()),
            expected);
}

/**
 * Given the sheet of the teller of the cashier shipped in assets/
 * When it is read
 * Then it holds TELLER_POSE_COUNT rows of TELLER_FRAMES_PER_POSE frames, one
 * row per pose, and no more
 */
TEST(CasinoSpriteConstants, TheTellerSheetHoldsTwoPosesOfEightFrames) {
  const sf::Vector2i expected(TELLER_FRAME_SIZE.x * TELLER_FRAMES_PER_POSE,
                              TELLER_FRAME_SIZE.y * TELLER_POSE_COUNT);

  EXPECT_EQ(sf::Vector2i(shippedSheet(TELLER_SHEET_ID).getSize()), expected);
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
