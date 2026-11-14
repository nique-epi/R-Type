#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/VertexArray.hpp>
#include <filesystem>
#include "BiomeConstants.hpp"
#include "BiomeDecorFixtures.hpp"
#include "BiomeDecorTestConstants.hpp"
#include "BiomeMap.hpp"
#include "BiomeMapTestConstants.hpp"
#include "ClientException.hpp"

using rtype::client::BiomeMap;
using rtype::client::BiomeMapException;
using rtype::client::FOSSE_LAYERS;
using rtype::client::FOSSE_PROJECT_ID;
using ::testing::ElementsAreArray;

/**
 * Given the project of the Pit
 * When its maps are read
 * Then each plane holds its six maps of 1440 pixels
 */
TEST(BiomeMap, ReadsEveryMapOfTheProject) {
  const BiomeMap map{filesOfTheAssets()(FOSSE_PROJECT_ID), FOSSE_LAYERS};

  EXPECT_THAT(map.plane(NEAR_PLANE).widths(),
              ElementsAreArray(WIDTHS_OF_THE_MAPS_OF_THE_PIT));
}

/**
 * Given the project of the Pit
 * When its maps are read
 * Then the near plane of its first map holds a pair of triangles per tile
 */
TEST(BiomeMap, BuildsEveryTileOfALayer) {
  const BiomeMap map{filesOfTheAssets()(FOSSE_PROJECT_ID), FOSSE_LAYERS};

  EXPECT_EQ(
      map.plane(NEAR_PLANE).tilesOf(FIRST_MAP_OF_THE_PIT).getVertexCount(),
      VERTICES_OF_THE_FIRST_NEAR_LAYER);
}

/**
 * Given the project of the Pit, whose first near tile lies at (112, 0), where
 * the entrance of the cave begins, with its picture at (0, 0)
 * When its maps are read
 * Then that tile starts there in the map, and at the corner of the tileset
 */
TEST(BiomeMap, PlacesATileWhereTheProjectHasIt) {
  const BiomeMap map{filesOfTheAssets()(FOSSE_PROJECT_ID), FOSSE_LAYERS};

  const sf::VertexArray& tiles =
      map.plane(NEAR_PLANE).tilesOf(FIRST_MAP_OF_THE_PIT);

  EXPECT_EQ(tiles[FIRST_TILE_CORNER].position, WHERE_THE_ENTRANCE_BEGINS);
  EXPECT_EQ(tiles[FIRST_TILE_CORNER].texCoords, CORNER_OF_THE_TILESET);
}

/**
 * Given a project that does not exist
 * When its maps are read
 * Then the biome map refuses it
 */
TEST(BiomeMap, RefusesAMissingProject) {
  EXPECT_THROW(
      const BiomeMap map(std::filesystem::path(FILE_NOWHERE), FOSSE_LAYERS),
      BiomeMapException);
}

/**
 * Given a biome naming a layer the maps of the Pit lack
 * When its maps are read
 * Then the biome map refuses it
 */
TEST(BiomeMap, RefusesALayerTheMapsLack) {
  EXPECT_THROW(const BiomeMap map(filesOfTheAssets()(FOSSE_PROJECT_ID),
                                  LAYERS_THE_PIT_LACKS),
               BiomeMapException);
}
