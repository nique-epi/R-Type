#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <variant>
#include "BackgroundCatalog.hpp"
#include "BackgroundCatalogFixture.hpp"
#include "BackgroundCatalogTestConstants.hpp"
#include "BackgroundDescription.hpp"
#include "ClientException.hpp"
#include "LayerDescription.hpp"
#include "TileContent.hpp"

using rtype::client::BackgroundCatalog;
using rtype::client::BackgroundDescription;
using rtype::client::LayerDescription;
using rtype::client::MissingBackgroundFileException;
using rtype::client::TileContent;
using ::testing::AllOf;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::HasSubstr;
using ::testing::SizeIs;
using ::testing::ThrowsMessage;

/**
 * Given assets that hold backgrounds/space.toml, a sky of one planet
 * When the backgrounds of the assets are read
 * Then the background named space is that sky
 */
TEST_F(BackgroundCatalogFixture, FindsABackgroundByTheNameOfItsFile) {
  write(SPACE_OF_THE_ASSETS, A_SKY_OF_ONE_PLANET);
  write(PLANET_OF_THE_ASSETS, ANY_IMAGE);

  const BackgroundCatalog catalog = catalogOfTheAssets();

  EXPECT_EQ(firstLayerNameOf(catalog.named(SPACE_NAME)), PLANET_LAYER_NAME);
}

/**
 * Given assets that hold one background, space
 * When a background of another name is asked for
 * Then there is none
 */
TEST_F(BackgroundCatalogFixture, FindsNoBackgroundUnderAnotherName) {
  write(SPACE_OF_THE_ASSETS, A_SKY_OF_ONE_PLANET);
  write(PLANET_OF_THE_ASSETS, ANY_IMAGE);

  const BackgroundCatalog catalog = catalogOfTheAssets();

  EXPECT_EQ(catalog.named(UNWRITTEN_NAME), nullptr);
}

/**
 * Given assets with no folder of backgrounds
 * When their backgrounds are read
 * Then there is none, and nothing is thrown
 */
TEST_F(BackgroundCatalogFixture, AFolderWithoutBackgroundsHoldsNone) {
  const BackgroundCatalog catalog = catalogOfTheAssets();

  EXPECT_EQ(catalog.named(SPACE_NAME), nullptr);
}

/**
 * Given assets whose space is a sky of one planet, and a resource whose space
 * is a sky of one moon
 * When the backgrounds of both are read
 * Then the background named space is the sky of the resource
 */
TEST_F(BackgroundCatalogFixture, ALaterFolderReplacesABackgroundOfTheSameName) {
  write(SPACE_OF_THE_ASSETS, A_SKY_OF_ONE_PLANET);
  write(PLANET_OF_THE_ASSETS, ANY_IMAGE);
  write(SPACE_OF_THE_RESOURCE, A_SKY_OF_ONE_MOON);
  write(MOON_OF_THE_RESOURCE, ANY_IMAGE);

  const BackgroundCatalog catalog = catalogOfBothFolders();

  EXPECT_EQ(firstLayerNameOf(catalog.named(SPACE_NAME)), MOON_LAYER_NAME);
}

/**
 * Given assets that hold space, and a resource that holds nebula
 * When the backgrounds of both are read
 * Then both are found, each under its name
 */
TEST_F(BackgroundCatalogFixture, ALaterFolderAddsItsOwnBackgrounds) {
  write(SPACE_OF_THE_ASSETS, A_SKY_OF_ONE_PLANET);
  write(PLANET_OF_THE_ASSETS, ANY_IMAGE);
  write(NEBULA_OF_THE_RESOURCE, A_SKY_OF_ONE_MOON);
  write(MOON_OF_THE_RESOURCE, ANY_IMAGE);

  const BackgroundCatalog catalog = catalogOfBothFolders();

  EXPECT_EQ(firstLayerNameOf(catalog.named(SPACE_NAME)), PLANET_LAYER_NAME);
  EXPECT_EQ(firstLayerNameOf(catalog.named(NEBULA_NAME)), MOON_LAYER_NAME);
}

/**
 * Given a background whose image lies in the assets alone
 * When the backgrounds are read
 * Then the image keeps the asset id the file gives it
 */
TEST_F(BackgroundCatalogFixture, AFileOfTheAssetsKeepsItsAssetId) {
  write(SPACE_OF_THE_ASSETS, A_SKY_OF_ONE_PLANET);
  write(PLANET_OF_THE_ASSETS, ANY_IMAGE);

  const BackgroundCatalog catalog = catalogOfBothFolders();

  EXPECT_EQ(firstImageFileOf(catalog.named(SPACE_NAME)), PLANET_ASSET_ID);
}

/**
 * Given a background of a resource whose image lies in that resource alone
 * When the backgrounds are read
 * Then the image is named by its whole path in the resource
 */
TEST_F(BackgroundCatalogFixture, AFileOfALaterFolderIsNamedByItsWholePath) {
  write(NEBULA_OF_THE_RESOURCE, A_SKY_OF_ONE_MOON);
  write(MOON_OF_THE_RESOURCE, ANY_IMAGE);

  const BackgroundCatalog catalog = catalogOfBothFolders();

  EXPECT_EQ(firstImageFileOf(catalog.named(NEBULA_NAME)),
            wholePathOf(MOON_OF_THE_RESOURCE));
}

/**
 * Given a background of the assets whose image lies in the assets and in a
 * resource too
 * When the backgrounds are read
 * Then the image is the one of the resource, named by its whole path
 */
TEST_F(BackgroundCatalogFixture, AFileALaterFolderHoldsTooIsTakenThere) {
  write(SPACE_OF_THE_ASSETS, A_SKY_OF_ONE_PLANET);
  write(PLANET_OF_THE_ASSETS, ANY_IMAGE);
  write(PLANET_OF_THE_RESOURCE, ANY_IMAGE);

  const BackgroundCatalog catalog = catalogOfBothFolders();

  EXPECT_EQ(firstImageFileOf(catalog.named(SPACE_NAME)),
            wholePathOf(PLANET_OF_THE_RESOURCE));
}

/**
 * Given a background whose image no content folder holds
 * When the backgrounds are read
 * Then a MissingBackgroundFileException names that image
 */
TEST_F(BackgroundCatalogFixture, RefusesAFileNoFolderHolds) {
  write(SPACE_OF_THE_ASSETS, A_SKY_OF_ONE_PLANET);

  EXPECT_THAT([this] { static_cast<void>(catalogOfBothFolders()); },
              ThrowsMessage<MissingBackgroundFileException>(
                  HasSubstr(std::string{PLANET_ASSET_ID})));
}

/**
 * Given assets that hold space, and a resource that holds nebula
 * When the names of their backgrounds are asked for
 * Then they are both, in alphabetical order
 */
TEST_F(BackgroundCatalogFixture, NamesItsBackgroundsInAlphabeticalOrder) {
  write(SPACE_OF_THE_ASSETS, A_SKY_OF_ONE_PLANET);
  write(PLANET_OF_THE_ASSETS, ANY_IMAGE);
  write(NEBULA_OF_THE_RESOURCE, A_SKY_OF_ONE_MOON);
  write(MOON_OF_THE_RESOURCE, ANY_IMAGE);

  const BackgroundCatalog catalog = catalogOfBothFolders();

  EXPECT_THAT(catalog.names(),
              ElementsAre(std::string{NEBULA_NAME}, std::string{SPACE_NAME}));
}

/**
 * Given the assets the repository ships
 * When the images of their backgrounds are asked for
 * Then they are nine, each once, the planet and the near walls among them
 */
TEST(BackgroundCatalog, ListsEveryImageItsBackgroundsNameOnce) {
  const BackgroundCatalog catalog = shippedBackgrounds();

  EXPECT_THAT(catalog.images(),
              AllOf(SizeIs(IMAGES_OF_THE_SHIPPED_BACKGROUNDS),
                    Contains(std::string{PLANET_ASSET_ID}),
                    Contains(std::string{NEAR_WALL_TILESET})));
}

/**
 * Given the assets the repository ships
 * When their backgrounds are read
 * Then the sky is among them, and its only group keeps its events to four at
 * once
 */
TEST(BackgroundCatalog, ReadsTheSkyTheRepositoryShips) {
  const BackgroundCatalog catalog = shippedBackgrounds();

  const BackgroundDescription* const sky = catalog.named(SHIPPED_SKY_NAME);
  ASSERT_NE(sky, nullptr);
  ASSERT_EQ(sky->groups.size(), 1U);
  EXPECT_EQ(sky->groups.front().mostAlive, SKY_EVENTS_AT_ONCE);
}

/**
 * Given the assets the repository ships
 * When their backgrounds are read
 * Then the Pit is among them: three layers of tiles, entered through the
 * last alone
 */
TEST(BackgroundCatalog, ReadsThePitTheRepositoryShips) {
  const BackgroundCatalog catalog = shippedBackgrounds();

  const BackgroundDescription* const pit = catalog.named(SHIPPED_PIT_NAME);
  ASSERT_NE(pit, nullptr);
  ASSERT_EQ(pit->layers.size(), LAYERS_OF_THE_PIT);
  for (const LayerDescription& layer : pit->layers) {
    EXPECT_TRUE(std::holds_alternative<TileContent>(layer.content));
    EXPECT_EQ(layer.entrance, &layer == &pit->layers.back());
  }
}
