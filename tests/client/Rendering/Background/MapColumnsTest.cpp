#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <span>
#include "MapColumn.hpp"
#include "MapColumns.hpp"
#include "MapColumnsTestConstants.hpp"

using rtype::client::MapColumn;
using rtype::client::mapColumns;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a biome that just began
 * When its maps are placed
 * Then none shows yet: the first one waits at the right edge of the sky
 */
TEST(MapColumns, ShowsNothingWhenTheBiomeBegins) {
  EXPECT_THAT(mapColumns(NOT_ENTERED, ONE_MAP_OF_THE_PIT), IsEmpty());
}

/**
 * Given a biome that entered the sky by 100 pixels
 * When its maps are placed
 * Then the first map starts 100 pixels left of the right edge of the sky
 */
TEST(MapColumns, SlidesTheFirstMapInFromTheRight) {
  EXPECT_THAT(mapColumns(ENTERED_A_HUNDRED_PIXELS, ONE_MAP_OF_THE_PIT),
              ElementsAre(MapColumn{
                  .map = FIRST_MAP,
                  .column = A_HUNDRED_PIXELS_LEFT_OF_THE_RIGHT_EDGE}));
}

/**
 * Given a biome that entered the sky by a little more than 100 pixels
 * When its maps are placed
 * Then the first map lands on the whole pixel of 100 pixels
 */
TEST(MapColumns, PlacesTheMapsOnWholePixels) {
  EXPECT_THAT(mapColumns(ENTERED_A_HUNDRED_PIXELS_AND_MORE, ONE_MAP_OF_THE_PIT),
              ElementsAre(MapColumn{
                  .map = FIRST_MAP,
                  .column = A_HUNDRED_PIXELS_LEFT_OF_THE_RIGHT_EDGE}));
}

/**
 * Given two maps of 300 pixels, entered by the sky and 250 pixels more
 * When they are placed
 * Then the first map leaves the sky by 250 pixels, the second follows it at
 * the seam, and the first comes back after the second
 */
TEST(MapColumns, LaysTheMapsEndToEndAndComesBackToTheFirst) {
  EXPECT_THAT(
      mapColumns(ENTERED_THE_SKY_AND_TWO_HUNDRED_FIFTY, TWO_SHORT_MAPS),
      ElementsAre(
          MapColumn{.map = FIRST_MAP,
                    .column = TWO_HUNDRED_FIFTY_LEFT_OF_THE_SKY},
          MapColumn{.map = SECOND_MAP,
                    .column = SECOND_SHORT_MAP_AFTER_THE_SEAM},
          MapColumn{.map = FIRST_MAP, .column = FIRST_SHORT_MAP_AGAIN}));
}

/**
 * Given two maps of 300 pixels, entered by the sky and both maps
 * When they are placed
 * Then the first map starts again at the left edge of the sky
 */
TEST(MapColumns, StartsAgainOnceEveryMapScrolledWhole) {
  EXPECT_THAT(
      mapColumns(ENTERED_THE_SKY_AND_BOTH_SHORT_MAPS, TWO_SHORT_MAPS),
      ElementsAre(
          MapColumn{.map = FIRST_MAP, .column = LEFT_EDGE_OF_THE_SKY},
          MapColumn{.map = SECOND_MAP, .column = ONE_SHORT_MAP_INTO_THE_SKY}));
}

/**
 * Given a biome without any map
 * When its maps are placed
 * Then none is
 */
TEST(MapColumns, PlacesNothingWithoutMaps) {
  EXPECT_THAT(mapColumns(ENTERED_A_HUNDRED_PIXELS, std::span<const int>{}),
              IsEmpty());
}

/**
 * Given a biome whose only map has no width
 * When its maps are placed
 * Then none is
 */
TEST(MapColumns, PlacesNothingOfMapsWithoutWidth) {
  EXPECT_THAT(mapColumns(ENTERED_A_HUNDRED_PIXELS, A_MAP_WITHOUT_WIDTH),
              IsEmpty());
}
