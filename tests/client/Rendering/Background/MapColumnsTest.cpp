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
 * Given a first map that starts on the right edge of the sky
 * When the maps are placed
 * Then none shows yet
 */
TEST(MapColumns, ShowsNothingFromTheRightEdge) {
  EXPECT_THAT(mapColumns(ON_THE_RIGHT_EDGE, ONE_MAP_OF_THE_PIT), IsEmpty());
}

/**
 * Given a first map that starts 100 pixels left of the right edge of the sky
 * When the maps are placed
 * Then it shows from there
 */
TEST(MapColumns, PlacesTheFirstMapWhereItStarts) {
  EXPECT_THAT(mapColumns(A_HUNDRED_PIXELS_INTO_THE_SKY, ONE_MAP_OF_THE_PIT),
              ElementsAre(MapColumn{
                  .map = FIRST_MAP,
                  .column = A_HUNDRED_PIXELS_LEFT_OF_THE_RIGHT_EDGE}));
}

/**
 * Given two maps of 300 pixels, the first one starting 250 pixels left of the
 * left edge of the sky
 * When they are placed
 * Then the first map leaves the sky by 250 pixels, the second follows it at
 * the seam, and the first comes back after the second
 */
TEST(MapColumns, LaysTheMapsEndToEndAndComesBackToTheFirst) {
  EXPECT_THAT(
      mapColumns(TWO_HUNDRED_FIFTY_LEFT_OF_THE_SKY_EDGE, TWO_SHORT_MAPS),
      ElementsAre(
          MapColumn{.map = FIRST_MAP,
                    .column = TWO_HUNDRED_FIFTY_LEFT_OF_THE_SKY},
          MapColumn{.map = SECOND_MAP,
                    .column = SECOND_SHORT_MAP_AFTER_THE_SEAM},
          MapColumn{.map = FIRST_MAP, .column = FIRST_SHORT_MAP_AGAIN}));
}

/**
 * Given two maps of 300 pixels, the first one starting both maps left of the
 * left edge of the sky
 * When they are placed
 * Then the first map starts again at the left edge of the sky
 */
TEST(MapColumns, StartsAgainOnceEveryMapScrolledWhole) {
  EXPECT_THAT(
      mapColumns(BOTH_SHORT_MAPS_LEFT_OF_THE_SKY, TWO_SHORT_MAPS),
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
  EXPECT_THAT(mapColumns(A_HUNDRED_PIXELS_INTO_THE_SKY, std::span<const int>{}),
              IsEmpty());
}

/**
 * Given a biome whose only map has no width
 * When its maps are placed
 * Then none is
 */
TEST(MapColumns, PlacesNothingOfMapsWithoutWidth) {
  EXPECT_THAT(mapColumns(A_HUNDRED_PIXELS_INTO_THE_SKY, A_MAP_WITHOUT_WIDTH),
              IsEmpty());
}
