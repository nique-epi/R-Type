#include <gtest/gtest.h>
#include <array>
#include <filesystem>
#include <vector>
#include "BackgroundFiles.hpp"
#include "BackgroundFilesTestConstants.hpp"
#include "ContentFile.hpp"
#include "EngineException.hpp"
#include "TemporaryFolder.hpp"

using rtype::engine::UnreadableDataFileException;
using rtype::game::backgroundFilesIn;
using rtype::game::testing::A_BACKGROUND_ALONE;
using rtype::game::testing::A_BACKGROUND_AND_ITS_IMAGE;
using rtype::game::testing::ASSETS_BELOW_THE_RESOURCES;
using rtype::game::testing::BACKGROUND_OF_THE_LATER_RESOURCE;
using rtype::game::testing::BACKGROUND_OF_WALLS_AND_DUST;
using rtype::game::testing::BACKGROUND_SHOWING_THE_MOON;
using rtype::game::testing::CAVE_BACKGROUND_FILE;
using rtype::game::testing::CAVE_BYTES;
using rtype::game::testing::CAVE_DUST;
using rtype::game::testing::CAVE_PROJECT;
using rtype::game::testing::CAVE_TILESET;
using rtype::game::testing::LATER_MOON_PIXELS;
using rtype::game::testing::LATER_RESOURCE;
using rtype::game::testing::MOON_BACKGROUND_FILE;
using rtype::game::testing::MOON_IMAGE;
using rtype::game::testing::MOON_PIXELS;
using rtype::game::testing::MOON_RESOURCE;
using rtype::game::testing::THE_CAVE_AND_ITS_THREE_FILES;
using rtype::identifiers::ContentFile;

/**
 * Given assets that hold a background and the image it names
 * When the backgrounds of the assets alone are listed
 * Then none is: every client has the backgrounds of its own assets
 */
TEST(BackgroundFiles, ListsNothingForTheAssetsAlone) {
  const TemporaryFolder root;
  const std::array folders{root.path() / ASSETS_BELOW_THE_RESOURCES};
  root.write(folders[0] / MOON_BACKGROUND_FILE, BACKGROUND_SHOWING_THE_MOON);
  root.write(folders[0] / MOON_IMAGE, MOON_PIXELS);

  EXPECT_TRUE(backgroundFilesIn(folders).empty());
}

/**
 * Given a resource without a folder of backgrounds
 * When the backgrounds of the assets and of the resource are listed
 * Then none is
 */
TEST(BackgroundFiles, ListsNothingForAResourceWithoutBackgrounds) {
  const TemporaryFolder root;
  const std::array folders{root.path() / ASSETS_BELOW_THE_RESOURCES,
                           root.path() / MOON_RESOURCE};

  EXPECT_TRUE(backgroundFilesIn(folders).empty());
}

/**
 * Given a resource that holds a background and the image it names
 * When the backgrounds are listed
 * Then both files are, in the order of their paths, with their bytes
 */
TEST(BackgroundFiles, ListsABackgroundOfAResourceAndTheImageItHolds) {
  const TemporaryFolder root;
  const std::array folders{root.path() / ASSETS_BELOW_THE_RESOURCES,
                           root.path() / MOON_RESOURCE};
  root.write(folders[1] / MOON_BACKGROUND_FILE, BACKGROUND_SHOWING_THE_MOON);
  root.write(folders[1] / MOON_IMAGE, MOON_PIXELS);

  const std::vector<ContentFile> files = backgroundFilesIn(folders);

  ASSERT_EQ(files.size(), A_BACKGROUND_AND_ITS_IMAGE);
  EXPECT_EQ(files[0].path, MOON_BACKGROUND_FILE);
  EXPECT_EQ(files[0].bytes, BACKGROUND_SHOWING_THE_MOON);
  EXPECT_EQ(files[1].path, MOON_IMAGE);
  EXPECT_EQ(files[1].bytes, MOON_PIXELS);
}

/**
 * Given a resource whose background names an image only the assets hold
 * When the backgrounds are listed
 * Then the background alone is: every client has the image already
 */
TEST(BackgroundFiles, LeavesOutAFileOnlyTheAssetsHold) {
  const TemporaryFolder root;
  const std::array folders{root.path() / ASSETS_BELOW_THE_RESOURCES,
                           root.path() / MOON_RESOURCE};
  root.write(folders[0] / MOON_IMAGE, MOON_PIXELS);
  root.write(folders[1] / MOON_BACKGROUND_FILE, BACKGROUND_SHOWING_THE_MOON);

  const std::vector<ContentFile> files = backgroundFilesIn(folders);

  ASSERT_EQ(files.size(), A_BACKGROUND_ALONE);
  EXPECT_EQ(files[0].path, MOON_BACKGROUND_FILE);
}

/**
 * Given two resources that hold a background of the same name
 * When the backgrounds are listed
 * Then the background is the one of the later resource
 */
TEST(BackgroundFiles, ALaterResourceReplacesABackgroundOfTheSameName) {
  const TemporaryFolder root;
  const std::array folders{root.path() / ASSETS_BELOW_THE_RESOURCES,
                           root.path() / MOON_RESOURCE,
                           root.path() / LATER_RESOURCE};
  root.write(folders[0] / MOON_IMAGE, MOON_PIXELS);
  root.write(folders[1] / MOON_BACKGROUND_FILE, BACKGROUND_SHOWING_THE_MOON);
  root.write(folders[2] / MOON_BACKGROUND_FILE,
             BACKGROUND_OF_THE_LATER_RESOURCE);

  const std::vector<ContentFile> files = backgroundFilesIn(folders);

  ASSERT_EQ(files.size(), A_BACKGROUND_ALONE);
  EXPECT_EQ(files[0].bytes, BACKGROUND_OF_THE_LATER_RESOURCE);
}

/**
 * Given a background of a resource, and an image it names that this
 * resource and a later one both hold
 * When the backgrounds are listed
 * Then the image is the one of the later resource
 */
TEST(BackgroundFiles, ReadsAFileFromTheLastResourceThatHoldsIt) {
  const TemporaryFolder root;
  const std::array folders{root.path() / ASSETS_BELOW_THE_RESOURCES,
                           root.path() / MOON_RESOURCE,
                           root.path() / LATER_RESOURCE};
  root.write(folders[1] / MOON_BACKGROUND_FILE, BACKGROUND_SHOWING_THE_MOON);
  root.write(folders[1] / MOON_IMAGE, MOON_PIXELS);
  root.write(folders[2] / MOON_IMAGE, LATER_MOON_PIXELS);

  const std::vector<ContentFile> files = backgroundFilesIn(folders);

  ASSERT_EQ(files.size(), A_BACKGROUND_AND_ITS_IMAGE);
  EXPECT_EQ(files[1].bytes, LATER_MOON_PIXELS);
}

/**
 * Given a resource whose background shows tiles and particles drawn with a
 * sprite, and that holds the project, the tileset and the sprite
 * When the backgrounds are listed
 * Then the background and its three files are, in the order of their paths
 */
TEST(BackgroundFiles, ListsTheProjectTheTilesetAndTheSpritesOfABackground) {
  const TemporaryFolder root;
  const std::array folders{root.path() / ASSETS_BELOW_THE_RESOURCES,
                           root.path() / MOON_RESOURCE};
  root.write(folders[1] / CAVE_BACKGROUND_FILE, BACKGROUND_OF_WALLS_AND_DUST);
  root.write(folders[1] / CAVE_PROJECT, CAVE_BYTES);
  root.write(folders[1] / CAVE_TILESET, CAVE_BYTES);
  root.write(folders[1] / CAVE_DUST, CAVE_BYTES);

  const std::vector<ContentFile> files = backgroundFilesIn(folders);

  ASSERT_EQ(files.size(), THE_CAVE_AND_ITS_THREE_FILES);
  EXPECT_EQ(files[0].path, CAVE_BACKGROUND_FILE);
  EXPECT_EQ(files[1].path, CAVE_DUST);
  EXPECT_EQ(files[2].path, CAVE_PROJECT);
  EXPECT_EQ(files[3].path, CAVE_TILESET);
}

/**
 * Given a resource whose background names an image no folder holds
 * When the backgrounds are listed
 * Then the listing is refused for that file
 */
TEST(BackgroundFiles, RefusesABackgroundNamingAFileNoFolderHolds) {
  const TemporaryFolder root;
  const std::array folders{root.path() / ASSETS_BELOW_THE_RESOURCES,
                           root.path() / MOON_RESOURCE};
  root.write(folders[1] / MOON_BACKGROUND_FILE, BACKGROUND_SHOWING_THE_MOON);

  EXPECT_THROW(static_cast<void>(backgroundFilesIn(folders)),
               UnreadableDataFileException);
}
