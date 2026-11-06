#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <filesystem>
#include "AssetFolder.hpp"
#include "AssetFolderTestConstants.hpp"
#include "EngineException.hpp"
#include "PathText.hpp"
#include "TemporaryFolder.hpp"

using rtype::engine::AssetFolderNotFoundException;
using rtype::engine::findAssetFolder;
using rtype::logging::genericText;
using ::testing::AllOf;
using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

/**
 * Given an assets folder next to the executable folder
 * When the assets folder is searched from that folder
 * Then that assets folder is used
 */
TEST(AssetFolder, FindsTheFolderNextToTheExecutable) {
  const TemporaryFolder executableFolder;
  std::filesystem::create_directories(executableFolder.path() / ASSETS_FOLDER);

  EXPECT_EQ(findAssetFolder(executableFolder.path()),
            executableFolder.path() / ASSETS_FOLDER);
}

/**
 * Given an assets folder one level above the executable folder, as with the
 * Release folder of the Visual Studio generator
 * When the assets folder is searched from the executable folder
 * Then the assets folder above is used
 */
TEST(AssetFolder, FindsTheFolderOneLevelAboveTheExecutable) {
  const TemporaryFolder repository;
  const std::filesystem::path executableFolder =
      repository.path() / VISUAL_STUDIO_OUTPUT_FOLDER;
  std::filesystem::create_directories(executableFolder);
  std::filesystem::create_directories(repository.path() / ASSETS_FOLDER);

  EXPECT_EQ(findAssetFolder(executableFolder),
            repository.path() / ASSETS_FOLDER);
}

/**
 * Given an assets folder next to the executable folder and another one level
 * above
 * When the assets folder is searched
 * Then the one next to the executable is used
 */
TEST(AssetFolder, PrefersTheFolderNextToTheExecutable) {
  const TemporaryFolder repository;
  const std::filesystem::path executableFolder =
      repository.path() / VISUAL_STUDIO_OUTPUT_FOLDER;
  std::filesystem::create_directories(executableFolder / ASSETS_FOLDER);
  std::filesystem::create_directories(repository.path() / ASSETS_FOLDER);

  EXPECT_EQ(findAssetFolder(executableFolder),
            executableFolder / ASSETS_FOLDER);
}

/**
 * Given a file named assets next to the executable and an assets folder one
 * level above
 * When the assets folder is searched
 * Then the file is skipped and the folder above is used
 */
TEST(AssetFolder, SkipsAFileNamedAssets) {
  const TemporaryFolder repository;
  const std::filesystem::path executableFolder =
      repository.path() / VISUAL_STUDIO_OUTPUT_FOLDER;
  repository.write(
      std::filesystem::path(VISUAL_STUDIO_OUTPUT_FOLDER) / ASSETS_FOLDER, "");
  std::filesystem::create_directories(repository.path() / ASSETS_FOLDER);

  EXPECT_EQ(findAssetFolder(executableFolder),
            repository.path() / ASSETS_FOLDER);
}

/**
 * Given a macOS app whose assets folder sits in Contents/Resources
 * When the assets folder is searched from Contents/MacOS
 * Then the assets folder of the resources is used
 */
TEST(AssetFolder, FindsTheFolderInTheResourcesOfAnApp) {
  const TemporaryFolder app;
  const std::filesystem::path executableFolder =
      app.path() / APP_EXECUTABLE_FOLDER;
  std::filesystem::create_directories(executableFolder);
  std::filesystem::create_directories(executableFolder.parent_path() /
                                      APP_RESOURCES_NAME / ASSETS_FOLDER);

  EXPECT_EQ(
      findAssetFolder(executableFolder),
      executableFolder.parent_path() / APP_RESOURCES_NAME / ASSETS_FOLDER);
}

/**
 * Given an assets folder one level above the executable folder of an app and
 * another in its resources
 * When the assets folder is searched
 * Then the one above the executable is used
 */
TEST(AssetFolder, PrefersTheFolderAboveTheExecutableToTheResources) {
  const TemporaryFolder app;
  const std::filesystem::path executableFolder =
      app.path() / APP_EXECUTABLE_FOLDER;
  std::filesystem::create_directories(executableFolder);
  std::filesystem::create_directories(executableFolder.parent_path() /
                                      ASSETS_FOLDER);
  std::filesystem::create_directories(executableFolder.parent_path() /
                                      APP_RESOURCES_NAME / ASSETS_FOLDER);

  EXPECT_EQ(findAssetFolder(executableFolder),
            executableFolder.parent_path() / ASSETS_FOLDER);
}

/**
 * Given no assets folder next to, above or beside the executable folder
 * When the assets folder is searched
 * Then the error names the three searched folders
 */
TEST(AssetFolder, NamesTheThreeSearchedFoldersWhenNoneExists) {
  const TemporaryFolder repository;
  const std::filesystem::path executableFolder =
      repository.path() / VISUAL_STUDIO_OUTPUT_FOLDER;
  std::filesystem::create_directories(executableFolder);

  EXPECT_THAT(
      [&executableFolder] {
        static_cast<void>(findAssetFolder(executableFolder));
      },
      ThrowsMessage<AssetFolderNotFoundException>(
          AllOf(HasSubstr(genericText(executableFolder / ASSETS_FOLDER)),
                HasSubstr(genericText(repository.path() / ASSETS_FOLDER)),
                HasSubstr(genericText(repository.path() / APP_RESOURCES_NAME /
                                      ASSETS_FOLDER)))));
}
