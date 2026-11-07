#include <gtest/gtest.h>
#include <array>
#include <filesystem>
#include <string>
#include <vector>
#include "ContentFolders.hpp"
#include "ContentFoldersTestConstants.hpp"
#include "EngineException.hpp"
#include "ResourceFixtures.hpp"
#include "ResourcePlaces.hpp"
#include "TemporaryFolder.hpp"

using rtype::engine::contentFoldersOf;
using rtype::engine::ResourceDependencyCycleException;
using rtype::engine::ResourceNotFoundException;
using rtype::engine::ResourcePlaces;

/**
 * Given no resource to start
 * When the content folders are laid
 * Then the assets folder is the only one
 */
TEST(ContentFolders, AreTheAssetsAloneWhenNoResourceStarts) {
  const TemporaryFolder root;
  const ResourcePlaces places{
      .assetFolder = root.path() / TEST_ASSET_FOLDER,
      .resourceFolder = root.path() / TEST_RESOURCE_FOLDER};

  const std::vector<std::filesystem::path> folders =
      contentFoldersOf(places, {});

  EXPECT_EQ(folders, std::vector<std::filesystem::path>{places.assetFolder});
}

/**
 * Given a boss resource that depends on a crimson resource
 * When only the boss is started
 * Then the assets come first, then crimson, then the boss
 */
TEST(ContentFolders, LayADependencyBeforeTheResourceThatNeedsIt) {
  const TemporaryFolder root;
  root.write(CRIMSON_MANIFEST, MANIFEST_NEEDING_NOTHING);
  root.write(BOSS_MANIFEST, MANIFEST_NEEDING_CRIMSON);
  const ResourcePlaces places{
      .assetFolder = root.path() / TEST_ASSET_FOLDER,
      .resourceFolder = root.path() / TEST_RESOURCE_FOLDER};

  const std::vector<std::filesystem::path> folders =
      contentFoldersOf(places, textsOf(ONLY_THE_BOSS));

  EXPECT_EQ(folders,
            (std::vector<std::filesystem::path>{
                places.assetFolder, places.resourceFolder / CRIMSON_RESOURCE,
                places.resourceFolder / BOSS_RESOURCE}));
}

/**
 * Given a boss resource that depends on a crimson resource
 * When the boss, then crimson, are started
 * Then crimson is laid once, before the boss
 */
TEST(ContentFolders, LayAResourceOnceWhenTwoLinesBringIt) {
  const TemporaryFolder root;
  root.write(CRIMSON_MANIFEST, MANIFEST_NEEDING_NOTHING);
  root.write(BOSS_MANIFEST, MANIFEST_NEEDING_CRIMSON);
  const ResourcePlaces places{
      .assetFolder = root.path() / TEST_ASSET_FOLDER,
      .resourceFolder = root.path() / TEST_RESOURCE_FOLDER};

  const std::vector<std::filesystem::path> folders =
      contentFoldersOf(places, textsOf(THE_BOSS_THEN_CRIMSON));

  ASSERT_EQ(folders.size(), FOLDERS_FOR_THE_BOSS);
  EXPECT_EQ(folders[1], places.resourceFolder / CRIMSON_RESOURCE);
}

/**
 * Given a resource folder that holds no resource of that name
 * When that resource is started
 * Then the start is refused for the missing resource
 */
TEST(ContentFolders, RefuseAResourceWithoutAManifest) {
  const TemporaryFolder root;
  const ResourcePlaces places{
      .assetFolder = root.path() / TEST_ASSET_FOLDER,
      .resourceFolder = root.path() / TEST_RESOURCE_FOLDER};
  const std::vector<std::string> names = textsOf(std::array{MISSING_RESOURCE});

  EXPECT_THROW(static_cast<void>(contentFoldersOf(places, names)),
               ResourceNotFoundException);
}

/**
 * Given two resources that depend on each other
 * When one of them is started
 * Then the start is refused for the circle
 */
TEST(ContentFolders, RefuseResourcesThatDependOnEachOther) {
  const TemporaryFolder root;
  root.write(HEN_MANIFEST, MANIFEST_NEEDING_THE_EGG);
  root.write(EGG_MANIFEST, MANIFEST_NEEDING_THE_HEN);
  const ResourcePlaces places{
      .assetFolder = root.path() / TEST_ASSET_FOLDER,
      .resourceFolder = root.path() / TEST_RESOURCE_FOLDER};
  const std::vector<std::string> names = textsOf(std::array{HEN_RESOURCE});

  EXPECT_THROW(static_cast<void>(contentFoldersOf(places, names)),
               ResourceDependencyCycleException);
}
