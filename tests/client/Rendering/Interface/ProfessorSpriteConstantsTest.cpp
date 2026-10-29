#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <cstddef>
#include "ProfessorSpriteConstants.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::PROFESSOR_FACING_LEFT_FRAMES;
using rtype::client::PROFESSOR_FACING_RIGHT_FRAMES;
using rtype::client::PROFESSOR_SHEET_ID;
using rtype::game::SpriteFrame;

/**
 * Given the professor sheet shipped in assets/
 * When each of its frames is placed on it
 * Then every frame lies inside the sheet
 */
TEST(ProfessorSpriteConstants, EveryFrameLiesInsideTheProfessorSheet) {
  const sf::Image sheet = shippedSheet(PROFESSOR_SHEET_ID);
  for (const auto& row :
       {PROFESSOR_FACING_LEFT_FRAMES, PROFESSOR_FACING_RIGHT_FRAMES}) {
    for (const SpriteFrame frame : row) {
      SCOPED_TRACE(frame.top);
      EXPECT_TRUE(liesInside(frame, sheet.getSize()));
    }
  }
}

/**
 * Given the professor sheet shipped in assets/
 * When each of its frames is read
 * Then every frame shows a drawing, and none cuts one
 */
TEST(ProfessorSpriteConstants, EveryFrameHoldsAWholeDrawing) {
  const sf::Image sheet = shippedSheet(PROFESSOR_SHEET_ID);
  for (const auto& row :
       {PROFESSOR_FACING_LEFT_FRAMES, PROFESSOR_FACING_RIGHT_FRAMES}) {
    for (const SpriteFrame frame : row) {
      SCOPED_TRACE(frame.top);
      EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
      EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
    }
  }
}

/**
 * Given the professor sheet shipped in assets/
 * When a frame facing right is compared with the frame above it, mirrored
 * Then no pixel differs, so the professor looks the same both ways
 */
TEST(ProfessorSpriteConstants, TheRightRowMirrorsTheLeftRow) {
  const sf::Image sheet = shippedSheet(PROFESSOR_SHEET_ID);
  for (std::size_t frame = 0; frame < PROFESSOR_FACING_LEFT_FRAMES.size();
       ++frame) {
    SCOPED_TRACE(frame);
    EXPECT_EQ(mirrorMismatchCount(sheet, PROFESSOR_FACING_LEFT_FRAMES.at(frame),
                                  PROFESSOR_FACING_RIGHT_FRAMES.at(frame)),
              0);
  }
}
