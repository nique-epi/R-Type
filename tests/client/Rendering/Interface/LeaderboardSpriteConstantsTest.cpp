#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <algorithm>
#include <cstddef>
#include "LeaderboardSpriteConstants.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::FLAG_FRAMES;
using rtype::client::GUARD_FACING_LEFT_FRAMES;
using rtype::client::GUARD_FACING_RIGHT_FRAMES;
using rtype::client::LEADERBOARD_SHEET_ID;
using rtype::game::SpriteFrame;

/**
 * Given the sheet of the leaderboard shipped in assets/
 * When each banner is placed on it
 * Then it lies inside the sheet, shows a drawing, and cuts none
 */
TEST(LeaderboardSpriteConstants, EveryBannerHoldsAWholeDrawing) {
  const sf::Image sheet = shippedSheet(LEADERBOARD_SHEET_ID);
  for (const SpriteFrame frame : FLAG_FRAMES) {
    SCOPED_TRACE(frame.left);
    EXPECT_TRUE(liesInside(frame, sheet.getSize()));
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
  }
}

/**
 * Given the sheet of the leaderboard shipped in assets/
 * When each frame of the guard is placed on it
 * Then it lies inside the sheet, shows a drawing, and cuts none
 */
TEST(LeaderboardSpriteConstants, EveryGuardHoldsAWholeDrawing) {
  const sf::Image sheet = shippedSheet(LEADERBOARD_SHEET_ID);
  for (const auto& row :
       {GUARD_FACING_LEFT_FRAMES, GUARD_FACING_RIGHT_FRAMES}) {
    for (const SpriteFrame frame : row) {
      SCOPED_TRACE(frame.top);
      EXPECT_TRUE(liesInside(frame, sheet.getSize()));
      EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
      EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
    }
  }
}

/**
 * Given the sheet of the leaderboard shipped in assets/
 * When a guard facing right is compared with the guard above it, mirrored
 * Then no pixel differs
 */
TEST(LeaderboardSpriteConstants, TheRightGuardsMirrorTheLeftGuards) {
  const sf::Image sheet = shippedSheet(LEADERBOARD_SHEET_ID);
  for (std::size_t frame = 0; frame < GUARD_FACING_LEFT_FRAMES.size();
       ++frame) {
    SCOPED_TRACE(frame);
    EXPECT_EQ(mirrorMismatchCount(sheet, GUARD_FACING_LEFT_FRAMES.at(frame),
                                  GUARD_FACING_RIGHT_FRAMES.at(frame)),
              0);
  }
}

/**
 * Given the banners of the sheet, a loop
 * When the last frame leads to the first
 * Then no more pixels change than between any two frames of the loop: no jump
 */
TEST(LeaderboardSpriteConstants, TheLoopOfTheBannerClosesWithoutAJump) {
  const sf::Image sheet = shippedSheet(LEADERBOARD_SHEET_ID);
  int largestStep = 0;
  for (std::size_t frame = 0; frame + 1 < FLAG_FRAMES.size(); ++frame) {
    largestStep =
        std::max(largestStep, changedPixelCount(sheet, FLAG_FRAMES.at(frame),
                                                FLAG_FRAMES.at(frame + 1)));
  }

  EXPECT_LE(changedPixelCount(sheet, FLAG_FRAMES.back(), FLAG_FRAMES.front()),
            largestStep);
}
