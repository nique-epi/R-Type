#include <gtest/gtest.h>
#include <SFML/Graphics/VertexArray.hpp>
#include "MapPlane.hpp"
#include "MapPlaneTestConstants.hpp"
#include "MapTileVertices.hpp"
#include "MapTileVerticesTestConstants.hpp"

using rtype::client::MapPlane;
using rtype::client::tileVertices;

/**
 * Given a plane whose only map holds one tile at column 32
 * When the column of its first tile is asked for
 * Then it is 32
 */
TEST(MapPlane, FindsTheColumnOfTheFirstTileOfAMap) {
  MapPlane plane;
  plane.addMap(tileVertices(ONE_TILE, TILE_SIZE), MAP_WIDTH);

  EXPECT_EQ(plane.firstTileColumnOf(0), TILE_POSITION.x);
}

/**
 * Given a plane whose only map, 300 pixels wide, holds no tile
 * When the column of its first tile is asked for
 * Then it is its width: the map is open from end to end
 */
TEST(MapPlane, OpensAMapWithoutTilesOverItsWholeWidth) {
  MapPlane plane;
  plane.addMap(sf::VertexArray{}, MAP_WIDTH);

  EXPECT_EQ(plane.firstTileColumnOf(0), MAP_WIDTH);
}
