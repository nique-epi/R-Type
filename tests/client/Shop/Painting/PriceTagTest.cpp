#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include "PriceTag.hpp"
#include "PriceTagTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::paintPriceTag;
using rtype::client::priceTagWidth;
using rtype::client::testing::DEAREST_HULL_PRICE;
using rtype::client::testing::DEAREST_HULL_TAG_WIDTH;
using rtype::client::testing::FOUR_DIGIT_PRICE;
using rtype::client::testing::FOUR_DIGIT_TAG_WIDTH;
using rtype::client::testing::TAG_AREA;
using rtype::client::testing::TAG_CORNER;

/**
 * Given a price of four digits
 * When the width of its tag is asked
 * Then it leaves room for the icon in a cell of 32 pixels: 23 pixels
 */
TEST(PriceTag, ATagOfFourDigitsIsNarrowerThanACell) {
  EXPECT_EQ(priceTagWidth(FOUR_DIGIT_PRICE), FOUR_DIGIT_TAG_WIDTH);
}

/**
 * Given the price of the dearest hull, five digits
 * When the width of its tag is asked
 * Then it fits the stall of a hull, 32 pixels wide: 27 pixels
 */
TEST(PriceTag, TheTagOfTheDearestHullFitsItsStall) {
  EXPECT_EQ(priceTagWidth(DEAREST_HULL_PRICE), DEAREST_HULL_TAG_WIDTH);
}

/**
 * Given a tag painted with its top-right corner at a pixel
 * When what it painted is read
 * Then every pixel lies inside the tag, which ends at that corner
 */
TEST(PriceTag, ATagPaintsOnlyInsideItsLabel) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintPriceTag(canvas, TAG_CORNER, FOUR_DIGIT_PRICE);

  ASSERT_FALSE(litPixelsOf(record).empty());
  for (const auto& [column, row] : litPixelsOf(record)) {
    EXPECT_TRUE(TAG_AREA.contains(sf::Vector2i(column, row)));
  }
}
