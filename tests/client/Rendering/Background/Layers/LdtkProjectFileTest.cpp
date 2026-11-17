#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/VertexArray.hpp>
#include "ClientException.hpp"
#include "LdtkProjectFile.hpp"
#include "LdtkProjectFileFixtures.hpp"
#include "LdtkProjectFileTestConstants.hpp"
#include "MapPlane.hpp"
#include "MapTileVerticesTestConstants.hpp"

using rtype::client::BiomeMapException;
using rtype::client::LdtkProjectFile;
using rtype::client::MapPlane;
using ::testing::ElementsAreArray;

/**
 * Given the project of the Pit, which holds six maps of 1440 pixels
 * When its near tile layer is read
 * Then the plane holds the six maps, in the order of the project
 */
TEST(LdtkProjectFile, ReadsEveryMapOfTheProject) {
  const LdtkProjectFile pit{shippedFile(PIT_PROJECT)};

  const MapPlane plane = pit.tilePlane(NEAR_TILE_LAYER);

  EXPECT_THAT(plane.widths(), ElementsAreArray(WIDTHS_OF_THE_MAPS_OF_THE_PIT));
}

/**
 * Given the first map of the Pit, whose near layer holds 661 tiles
 * When that layer is read
 * Then the map holds two triangles per tile
 */
TEST(LdtkProjectFile, BuildsEveryTileOfALayer) {
  const LdtkProjectFile pit{shippedFile(PIT_PROJECT)};

  const MapPlane plane = pit.tilePlane(NEAR_TILE_LAYER);

  EXPECT_EQ(plane.tilesOf(FIRST_MAP_OF_THE_PIT).getVertexCount(),
            VERTICES_OF_THE_FIRST_NEAR_LAYER);
}

/**
 * Given the first map of the Pit, whose first near tile lies at (112, 0),
 * its picture at (0, 0) in the tileset
 * When the near layer is read
 * Then the first corner of the map lies there, textured from there
 */
TEST(LdtkProjectFile, PlacesATileWhereTheProjectHasIt) {
  const LdtkProjectFile pit{shippedFile(PIT_PROJECT)};

  const MapPlane plane = pit.tilePlane(NEAR_TILE_LAYER);

  const sf::VertexArray& tiles = plane.tilesOf(FIRST_MAP_OF_THE_PIT);
  EXPECT_EQ(tiles[FIRST_TILE_CORNER].position, WHERE_THE_ENTRANCE_BEGINS);
  EXPECT_EQ(tiles[FIRST_TILE_CORNER].texCoords, CORNER_OF_THE_TILESET);
}

/**
 * Given the near layer of the Pit, whose first map leaves its 112 first
 * columns empty
 * When where the first map begins is asked for
 * Then it is the column 112
 */
TEST(LdtkProjectFile, FindsWhereTheEntranceBegins) {
  const LdtkProjectFile pit{shippedFile(PIT_PROJECT)};

  const MapPlane plane = pit.tilePlane(NEAR_TILE_LAYER);

  EXPECT_EQ(plane.firstTileColumnOf(FIRST_MAP_OF_THE_PIT),
            COLUMN_WHERE_THE_ENTRANCE_BEGINS);
}

/**
 * Given a project no folder holds
 * When it is read
 * Then a BiomeMapException is thrown
 */
TEST(LdtkProjectFile, RefusesAMissingProject) {
  EXPECT_THROW(const LdtkProjectFile missing{shippedFile(MISSING_PROJECT)},
               BiomeMapException);
}

/**
 * Given the project of the Pit, and a layer none of its maps has
 * When that layer is read
 * Then a BiomeMapException is thrown
 */
TEST(LdtkProjectFile, RefusesALayerTheMapsLack) {
  const LdtkProjectFile pit{shippedFile(PIT_PROJECT)};

  EXPECT_THROW(static_cast<void>(pit.tilePlane(LAYER_THE_PIT_LACKS)),
               BiomeMapException);
}

/**
 * Given a project whose entity layer holds a crate with a tile and a marker
 * without
 * When that layer is read as props
 * Then its one map holds the two triangles of the crate alone
 */
TEST(LdtkProjectFile, ReadsOnePropPerEntityThatHasATile) {
  const LdtkProjectFile project{propProject()};

  const MapPlane plane = project.propPlane(PROP_LAYER);

  EXPECT_THAT(plane.widths(), ElementsAreArray(WIDTH_OF_THE_PROP_MAP));
  EXPECT_EQ(plane.tilesOf(THE_PROP_MAP).getVertexCount(), VERTICES_OF_ONE_PROP);
}

/**
 * Given a crate of 16 by 16 pixels placed at (104, 64) by the middle of its
 * bottom side, whose tile lies at (16, 0) in its tileset
 * When the layer is read as props
 * Then the crate covers (96, 48) to (112, 64), textured from that tile
 */
TEST(LdtkProjectFile, PlacesAPropByItsPivot) {
  const LdtkProjectFile project{propProject()};

  const MapPlane plane = project.propPlane(PROP_LAYER);

  const sf::VertexArray& prop = plane.tilesOf(THE_PROP_MAP);
  ASSERT_EQ(prop.getVertexCount(), VERTICES_OF_ONE_PROP);
  EXPECT_EQ(prop[FIRST_VERTEX].position, TOP_LEFT_OF_THE_CRATE);
  EXPECT_EQ(prop[THIRD_VERTEX].position, BOTTOM_RIGHT_OF_THE_CRATE);
  EXPECT_EQ(prop[FIRST_VERTEX].texCoords, TOP_LEFT_OF_ITS_TILE);
  EXPECT_EQ(prop[THIRD_VERTEX].texCoords, BOTTOM_RIGHT_OF_ITS_TILE);
}
