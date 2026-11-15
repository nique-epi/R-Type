#include <gtest/gtest.h>
#include <SFML/Graphics/VertexArray.hpp>
#include <array>
#include "MapTile.hpp"
#include "MapTileVertices.hpp"
#include "MapTileVerticesTestConstants.hpp"

using rtype::client::MapTile;
using rtype::client::tileVertices;

/**
 * Given one tile
 * When its vertices are built
 * Then it is two triangles, six corners
 */
TEST(MapTileVertices, DrawsATileAsTwoTriangles) {
  EXPECT_EQ(tileVertices(ONE_TILE, TILE_SIZE).getVertexCount(),
            VERTICES_OF_ONE_TILE);
}

/**
 * Given a tile at (32, 16) in its map
 * When its vertices are built
 * Then its first corner is there, and its third one 16 pixels right and down
 */
TEST(MapTileVertices, PlacesTheTileWhereTheMapHasIt) {
  const sf::VertexArray vertices = tileVertices(ONE_TILE, TILE_SIZE);

  EXPECT_EQ(vertices[FIRST_VERTEX].position, TOP_LEFT_IN_THE_MAP);
  EXPECT_EQ(vertices[THIRD_VERTEX].position, BOTTOM_RIGHT_IN_THE_MAP);
}

/**
 * Given a tile whose picture lies at (48, 0) in its tileset
 * When its vertices are built
 * Then they cut that square of 16 pixels out of the tileset
 */
TEST(MapTileVertices, TexturesTheTileFromItsSource) {
  const sf::VertexArray vertices = tileVertices(ONE_TILE, TILE_SIZE);

  EXPECT_EQ(vertices[FIRST_VERTEX].texCoords, TOP_LEFT_IN_THE_TILESET);
  EXPECT_EQ(vertices[THIRD_VERTEX].texCoords, BOTTOM_RIGHT_IN_THE_TILESET);
}

/**
 * Given a tile mirrored left to right
 * When its vertices are built
 * Then its top left corner shows the top right of its picture
 */
TEST(MapTileVertices, MirrorsATileLeftToRight) {
  const std::array tiles{
      MapTile{.position = TILE_POSITION, .source = TILE_SOURCE, .flipX = true}};

  const sf::VertexArray vertices = tileVertices(tiles, TILE_SIZE);

  EXPECT_EQ(vertices[FIRST_VERTEX].texCoords, TOP_RIGHT_IN_THE_TILESET);
}

/**
 * Given a tile mirrored top to bottom
 * When its vertices are built
 * Then its top left corner shows the bottom left of its picture
 */
TEST(MapTileVertices, MirrorsATileTopToBottom) {
  const std::array tiles{
      MapTile{.position = TILE_POSITION, .source = TILE_SOURCE, .flipY = true}};

  const sf::VertexArray vertices = tileVertices(tiles, TILE_SIZE);

  EXPECT_EQ(vertices[FIRST_VERTEX].texCoords, BOTTOM_LEFT_IN_THE_TILESET);
}
