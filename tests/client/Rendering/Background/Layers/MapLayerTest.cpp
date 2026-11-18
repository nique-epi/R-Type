#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "LayerTestConstants.hpp"
#include "MapLayer.hpp"
#include "MapLayerFixtures.hpp"
#include "MapLayerTestConstants.hpp"
#include "MapTileVerticesTestConstants.hpp"
#include "RecordedTiles.hpp"

using rtype::client::MapLayer;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a layer of two maps, 300 then 400 pixels wide, at its origin
 * When it is painted
 * Then the first map is drawn on the column 0 and the second on 300, each
 * with its tiles and the tileset of the layer
 */
TEST(MapLayer, LaysItsMapsEndToEndFromItsOrigin) {
  const MapLayer walls = layerOfTwoMaps();

  EXPECT_THAT(tilesPaintedBy(walls, AT_ITS_ORIGIN),
              ElementsAre(RecordedTiles{.tileset = std::string{WALL_TILESET},
                                        .vertices = VERTICES_OF_ONE_TILE,
                                        .column = 0},
                          RecordedTiles{.tileset = std::string{WALL_TILESET},
                                        .vertices = NO_VERTEX,
                                        .column = SECOND_MAP_AT_THE_ORIGIN}));
}

/**
 * Given a layer of two maps that moved 350 columns, past its first map
 * When it is painted
 * Then the second map is drawn on -50, and the first comes back after it
 * on 350
 */
TEST(MapLayer, ComesBackToItsFirstMapAfterTheLast) {
  const MapLayer walls = layerOfTwoMaps();

  EXPECT_THAT(tilesPaintedBy(walls, PAST_THE_FIRST_MAP),
              ElementsAre(RecordedTiles{.tileset = std::string{WALL_TILESET},
                                        .vertices = NO_VERTEX,
                                        .column = SECOND_MAP_PARTLY_GONE},
                          RecordedTiles{.tileset = std::string{WALL_TILESET},
                                        .vertices = VERTICES_OF_ONE_TILE,
                                        .column = FIRST_MAP_AGAIN}));
}

/**
 * Given a layer without maps
 * When it is painted
 * Then nothing is drawn
 */
TEST(MapLayer, ShowsNothingWithoutMaps) {
  const MapLayer walls = layerWithoutMaps();

  EXPECT_THAT(tilesPaintedBy(walls, AT_ITS_ORIGIN), IsEmpty());
}

/**
 * Given a layer whose first map has its leftmost tile on the column 32
 * When where it begins is asked for
 * Then it is that column
 */
TEST(MapLayer, BeginsWhereTheTilesOfItsFirstMapBegin) {
  const MapLayer walls = layerOfTwoMaps();

  EXPECT_EQ(walls.leftEdge(), COLUMN_OF_THE_FIRST_TILE);
}

/**
 * Given a layer without maps
 * When where it begins is asked for
 * Then it is its origin
 */
TEST(MapLayer, BeginsAtItsOriginWithoutMaps) {
  const MapLayer walls = layerWithoutMaps();

  EXPECT_EQ(walls.leftEdge(), 0);
}
