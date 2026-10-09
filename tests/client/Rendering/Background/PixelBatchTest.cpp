#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "PixelBatch.hpp"

using rtype::client::PixelBatch;

namespace {

constexpr sf::Vector2i PIXEL(7, 3);
constexpr sf::Vector2f MIDDLE_OF_PIXEL(7.5F, 3.5F);
constexpr sf::Vector2i OTHER_PIXEL(-2, 40);
constexpr sf::Color PIXEL_COLOR(10, 200, 30);
constexpr sf::Color OTHER_COLOR(250, 5, 90);

}  // namespace

/**
 * Given an empty pixel batch
 * When a pixel is plotted
 * Then the batch holds a single point, in the middle of that pixel
 */
TEST(PixelBatch, PlottedPixelBecomesOnePointInItsMiddle) {
  PixelBatch batch;

  batch.plot(PIXEL, PIXEL_COLOR);

  ASSERT_EQ(batch.points().getVertexCount(), 1U);
  EXPECT_EQ(batch.points()[0].position, MIDDLE_OF_PIXEL);
}

/**
 * Given an empty pixel batch
 * When a pixel is plotted in a color
 * Then its point carries that color
 */
TEST(PixelBatch, PointKeepsTheColorOfItsPixel) {
  PixelBatch batch;

  batch.plot(PIXEL, PIXEL_COLOR);

  ASSERT_EQ(batch.points().getVertexCount(), 1U);
  EXPECT_EQ(batch.points()[0].color, PIXEL_COLOR);
}

/**
 * Given an empty pixel batch
 * When two pixels are plotted one after the other
 * Then their points come in the same order, so the second is drawn over the
 * first
 */
TEST(PixelBatch, PointsKeepThePlottingOrder) {
  PixelBatch batch;

  batch.plot(PIXEL, PIXEL_COLOR);
  batch.plot(OTHER_PIXEL, OTHER_COLOR);

  ASSERT_EQ(batch.points().getVertexCount(), 2U);
  EXPECT_EQ(batch.points()[0].color, PIXEL_COLOR);
  EXPECT_EQ(batch.points()[1].color, OTHER_COLOR);
}

/**
 * Given a batch holding points
 * When it is cleared
 * Then it holds no point
 */
TEST(PixelBatch, ClearForgetsEveryPoint) {
  PixelBatch batch;
  batch.plot(PIXEL, PIXEL_COLOR);
  batch.plot(OTHER_PIXEL, OTHER_COLOR);

  batch.clear();

  EXPECT_EQ(batch.points().getVertexCount(), 0U);
}
