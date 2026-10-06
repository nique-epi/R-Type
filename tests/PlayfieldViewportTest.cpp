#include <gtest/gtest.h>
#include <array>
#include "PixelSize.hpp"
#include "PlayfieldConstants.hpp"
#include "PlayfieldViewport.hpp"

using rtype::client::fitPlayfieldInWindow;
using rtype::client::PixelSize;
using rtype::client::PlayfieldViewport;
using rtype::game::PLAYFIELD_HEIGHT;
using rtype::game::PLAYFIELD_WIDTH;

namespace {

constexpr auto PLAYFIELD_WIDTH_PIXELS =
    static_cast<unsigned int>(PLAYFIELD_WIDTH);
constexpr auto PLAYFIELD_HEIGHT_PIXELS =
    static_cast<unsigned int>(PLAYFIELD_HEIGHT);
constexpr float WHOLE = 1.0F;
constexpr float HALF = 0.5F;
constexpr float QUARTER = 0.25F;
constexpr float TOLERANCE = 1e-4F;

constexpr std::array ARBITRARY_WINDOW_SIZES{
    PixelSize{.width = 1920, .height = 1080},
    PixelSize{.width = 1366, .height = 768},
    PixelSize{.width = 1024, .height = 768},
    PixelSize{.width = 600, .height = 600},
    PixelSize{.width = 300, .height = 900},
    PixelSize{.width = 3000, .height = 10},
    PixelSize{.width = 1, .height = 1},
};

constexpr std::array WINDOWS_WITHOUT_AREA{
    PixelSize{.width = 0, .height = PLAYFIELD_HEIGHT_PIXELS},
    PixelSize{.width = PLAYFIELD_WIDTH_PIXELS, .height = 0},
    PixelSize{.width = 0, .height = 0},
};

}  // namespace

/**
 * Given a window with the proportions of the playfield
 * When the playfield is fitted in it
 * Then the playfield covers the whole window
 */
TEST(PlayfieldViewport, WindowWithPlayfieldProportionsIsCoveredEntirely) {
  const PlayfieldViewport viewport =
      fitPlayfieldInWindow(PLAYFIELD_WIDTH_PIXELS, PLAYFIELD_HEIGHT_PIXELS);

  EXPECT_FLOAT_EQ(viewport.left, 0.0F);
  EXPECT_FLOAT_EQ(viewport.top, 0.0F);
  EXPECT_FLOAT_EQ(viewport.width, WHOLE);
  EXPECT_FLOAT_EQ(viewport.height, WHOLE);
}

/**
 * Given a window twice as wide as the playfield and just as high
 * When the playfield is fitted in it
 * Then it takes the middle half, with a bar of a quarter on each side
 */
TEST(PlayfieldViewport, WiderWindowGetsEqualBarsOnTheLeftAndRight) {
  const PlayfieldViewport viewport =
      fitPlayfieldInWindow(2 * PLAYFIELD_WIDTH_PIXELS, PLAYFIELD_HEIGHT_PIXELS);

  EXPECT_FLOAT_EQ(viewport.left, QUARTER);
  EXPECT_FLOAT_EQ(viewport.top, 0.0F);
  EXPECT_FLOAT_EQ(viewport.width, HALF);
  EXPECT_FLOAT_EQ(viewport.height, WHOLE);
}

/**
 * Given a window twice as high as the playfield and just as wide
 * When the playfield is fitted in it
 * Then it takes the middle half, with a bar of a quarter above and below
 */
TEST(PlayfieldViewport, TallerWindowGetsEqualBarsAboveAndBelow) {
  const PlayfieldViewport viewport =
      fitPlayfieldInWindow(PLAYFIELD_WIDTH_PIXELS, 2 * PLAYFIELD_HEIGHT_PIXELS);

  EXPECT_FLOAT_EQ(viewport.left, 0.0F);
  EXPECT_FLOAT_EQ(viewport.top, QUARTER);
  EXPECT_FLOAT_EQ(viewport.width, WHOLE);
  EXPECT_FLOAT_EQ(viewport.height, HALF);
}

/**
 * Given windows of many sizes and proportions
 * When the playfield is fitted in each
 * Then the rectangle it gets, in pixels, has the proportions of the playfield
 */
TEST(PlayfieldViewport, PlayfieldKeepsItsProportionsInAnyWindow) {
  for (const PixelSize window : ARBITRARY_WINDOW_SIZES) {
    const PlayfieldViewport viewport =
        fitPlayfieldInWindow(window.width, window.height);

    const float shownWidth = viewport.width * static_cast<float>(window.width);
    const float shownHeight =
        viewport.height * static_cast<float>(window.height);
    EXPECT_NEAR(shownWidth / shownHeight, PLAYFIELD_WIDTH / PLAYFIELD_HEIGHT,
                TOLERANCE)
        << window.width << "x" << window.height;
  }
}

/**
 * Given windows of many sizes and proportions
 * When the playfield is fitted in each
 * Then the rectangle it gets never goes past an edge of the window
 */
TEST(PlayfieldViewport, PlayfieldStaysInsideAnyWindow) {
  for (const PixelSize window : ARBITRARY_WINDOW_SIZES) {
    const PlayfieldViewport viewport =
        fitPlayfieldInWindow(window.width, window.height);

    EXPECT_GE(viewport.left, 0.0F) << window.width << "x" << window.height;
    EXPECT_GE(viewport.top, 0.0F) << window.width << "x" << window.height;
    EXPECT_LE(viewport.left + viewport.width, WHOLE + TOLERANCE)
        << window.width << "x" << window.height;
    EXPECT_LE(viewport.top + viewport.height, WHOLE + TOLERANCE)
        << window.width << "x" << window.height;
  }
}

/**
 * Given a window with no width, no height, or neither
 * When the playfield is fitted in it
 * Then the playfield covers the whole window
 */
TEST(PlayfieldViewport, WindowWithoutAreaIsCoveredEntirely) {
  for (const PixelSize window : WINDOWS_WITHOUT_AREA) {
    const PlayfieldViewport viewport =
        fitPlayfieldInWindow(window.width, window.height);

    EXPECT_FLOAT_EQ(viewport.left, 0.0F)
        << window.width << "x" << window.height;
    EXPECT_FLOAT_EQ(viewport.top, 0.0F) << window.width << "x" << window.height;
    EXPECT_FLOAT_EQ(viewport.width, WHOLE)
        << window.width << "x" << window.height;
    EXPECT_FLOAT_EQ(viewport.height, WHOLE)
        << window.width << "x" << window.height;
  }
}
