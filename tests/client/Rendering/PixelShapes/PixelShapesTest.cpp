#include <gtest/gtest.h>
#include <vector>
#include "ColoredPixel.hpp"
#include "PixelShapes.hpp"
#include "PixelShapesFixtures.hpp"
#include "PixelShapesTestConstants.hpp"

using rtype::client::addDisc;
using rtype::client::addDither;
using rtype::client::addSegment;
using rtype::client::ColoredPixel;
using rtype::client::testing::distinctPlacesOf;
using rtype::client::testing::FOUR_BY_FOUR;
using rtype::client::testing::FOUR_BY_FOUR_PIXELS;
using rtype::client::testing::FOUR_RIGHT;
using rtype::client::testing::NO_RADIUS;
using rtype::client::testing::ORIGIN;
using rtype::client::testing::PIXELS_FROM_ORIGIN_TO_FOUR_RIGHT;
using rtype::client::testing::RADIUS_TWO;
using rtype::client::testing::RADIUS_TWO_PIXELS;
using rtype::client::testing::SHAPE_COLOR;
using rtype::client::testing::THICK_SEGMENT_PIXELS;
using rtype::client::testing::THREE_PIXELS_THICK;

/**
 * Given a segment that starts and ends on the same pixel
 * When its pixels are added
 * Then it is that one pixel
 */
TEST(PixelShapes, ASegmentOfOnePointIsOnePixel) {
  std::vector<ColoredPixel> pixels;

  addSegment(pixels, {.from = ORIGIN, .to = ORIGIN, .color = SHAPE_COLOR});

  ASSERT_EQ(pixels.size(), 1U);
  EXPECT_EQ(pixels.front().position, ORIGIN);
}

/**
 * Given a segment four pixels to the right
 * When its pixels are added
 * Then it holds five pixels, its first and its last included
 */
TEST(PixelShapes, ASegmentDrawsBothItsEnds) {
  std::vector<ColoredPixel> pixels;

  addSegment(pixels, {.from = ORIGIN, .to = FOUR_RIGHT, .color = SHAPE_COLOR});

  ASSERT_EQ(pixels.size(), PIXELS_FROM_ORIGIN_TO_FOUR_RIGHT);
  EXPECT_EQ(pixels.front().position, ORIGIN);
  EXPECT_EQ(pixels.back().position, FOUR_RIGHT);
}

/**
 * Given the same segment three pixels thick
 * When its pixels are added
 * Then it covers seven columns of three rows
 */
TEST(PixelShapes, AThickSegmentCoversAStripAroundItsLine) {
  std::vector<ColoredPixel> pixels;

  addSegment(pixels, {.from = ORIGIN,
                      .to = FOUR_RIGHT,
                      .thickness = THREE_PIXELS_THICK,
                      .color = SHAPE_COLOR});

  EXPECT_EQ(distinctPlacesOf(pixels), THICK_SEGMENT_PIXELS);
}

/**
 * Given a disc of radius 0
 * When its pixels are added
 * Then it is its centre alone
 */
TEST(PixelShapes, ADiscOfRadiusZeroIsOnePixel) {
  std::vector<ColoredPixel> pixels;

  addDisc(pixels, {.center = ORIGIN, .radius = 0, .color = SHAPE_COLOR});

  EXPECT_EQ(pixels.size(), 1U);
}

/**
 * Given a disc of a negative radius
 * When its pixels are added
 * Then nothing is added
 */
TEST(PixelShapes, ADiscOfANegativeRadiusIsNothing) {
  std::vector<ColoredPixel> pixels;

  addDisc(pixels,
          {.center = ORIGIN, .radius = NO_RADIUS, .color = SHAPE_COLOR});

  EXPECT_TRUE(pixels.empty());
}

/**
 * Given a disc of radius 2
 * When its pixels are added
 * Then it holds rows of 3, 5, 5, 5 and 3 pixels
 */
TEST(PixelShapes, ADiscOfRadiusTwoHoldsTwentyOnePixels) {
  std::vector<ColoredPixel> pixels;

  addDisc(pixels,
          {.center = ORIGIN, .radius = RADIUS_TWO, .color = SHAPE_COLOR});

  EXPECT_EQ(distinctPlacesOf(pixels), RADIUS_TWO_PIXELS);
}

/**
 * Given a patch of four by four pixels with a share of 0 everywhere
 * When it is dithered
 * Then no pixel lights
 */
TEST(PixelShapes, ADitherOfNothingLightsNoPixel) {
  std::vector<ColoredPixel> pixels;

  addDither(pixels, {.area = FOUR_BY_FOUR, .color = SHAPE_COLOR});

  EXPECT_TRUE(pixels.empty());
}

/**
 * Given a patch of four by four pixels with a share of 1 everywhere
 * When it is dithered
 * Then every pixel lights
 */
TEST(PixelShapes, ADitherOfAllLightsEveryPixel) {
  std::vector<ColoredPixel> pixels;

  addDither(pixels, {.area = FOUR_BY_FOUR,
                     .color = SHAPE_COLOR,
                     .topShare = 1.0F,
                     .bottomShare = 1.0F});

  EXPECT_EQ(pixels.size(), FOUR_BY_FOUR_PIXELS);
}
