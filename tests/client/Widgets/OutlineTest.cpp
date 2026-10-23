#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include "BackgroundColorConstants.hpp"
#include "Outline.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::Outline;
using rtype::client::STAR_WHITE;

namespace {

/** @brief A check box of the mockup, 7 pixels square, at the origin. */
constexpr sf::IntRect SEVEN_SQUARE({0, 0}, {7, 7});

}  // namespace

/**
 * Given a solid outline of a 7 pixel square
 * When it is painted
 * Then it lights the 24 pixels of its border and none inside
 */
TEST(OutlineTest, ASolidOutlineLightsItsBorderOnly) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  Outline(SEVEN_SQUARE, Outline::Line::Solid).paint(canvas, STAR_WHITE);
  const auto pixels = litPixelsOf(record);
  EXPECT_EQ(pixels.size(), 24U);
  EXPECT_FALSE(pixels.contains({3, 3}));
}

/**
 * Given a dashed outline of a 7 pixel square
 * When it is painted
 * Then it lights one pixel in two along its border, corners included: 12
 */
TEST(OutlineTest, ADashedOutlineLightsOnePixelInTwo) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  Outline(SEVEN_SQUARE, Outline::Line::Dashed).paint(canvas, STAR_WHITE);
  const auto pixels = litPixelsOf(record);
  EXPECT_EQ(pixels.size(), 12U);
  EXPECT_TRUE(pixels.contains({6, 6}));
  EXPECT_FALSE(pixels.contains({1, 0}));
}
