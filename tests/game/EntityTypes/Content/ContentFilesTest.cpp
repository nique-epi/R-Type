#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>
#include "ContentFile.hpp"
#include "ContentFiles.hpp"
#include "ContentFilesTestConstants.hpp"
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "GameException.hpp"
#include "ShippedEntitySheets.hpp"
#include "TemporaryFolder.hpp"

using rtype::engine::MissingDataFileKeyException;
using rtype::engine::UnreadableDataFileException;
using rtype::game::contentFilesIn;
using rtype::game::DuplicateEntityTypeException;
using rtype::game::EntitySheet;
using rtype::game::SharedContentFileException;
using rtype::game::testing::ART_SPRITE_SHEET;
using rtype::game::testing::ASSETS_OF_THE_STACK;
using rtype::game::testing::BYDO_PIXELS;
using rtype::game::testing::CRIMSON_PIXELS;
using rtype::game::testing::CRIMSON_SHEET_FILE;
using rtype::game::testing::CRIMSON_SPRITE_SHEET;
using rtype::game::testing::DRIFTER_SHEET_FILE;
using rtype::game::testing::FILES_OF_ONE_SHEET_AND_ITS_IMAGE;
using rtype::game::testing::FILES_OF_THE_STACK;
using rtype::game::testing::FILES_OF_TWO_SHEETS_AND_ONE_IMAGE;
using rtype::game::testing::HUD_PIXELS;
using rtype::game::testing::LISTED_ONCE;
using rtype::game::testing::RESOURCE_OF_THE_STACK;
using rtype::game::testing::SHARED_SPRITE_SHEET;
using rtype::game::testing::SHEET_FILE_EXTENSION;
using rtype::game::testing::SHEET_FOLDER_PREFIX;
using rtype::game::testing::SHEET_NAMING_A_MISSING_IMAGE;
using rtype::game::testing::SHEET_NAMING_THE_ART;
using rtype::game::testing::SHEET_NAMING_THE_BYDOS;
using rtype::game::testing::SHEET_NAMING_THE_CRIMSON_BYDOS;
using rtype::game::testing::SHEET_WITHOUT_SPRITE_SHEET;
using rtype::game::testing::SHIPPED_BYDOMANCER_SPRITES;
using rtype::game::testing::SHIPPED_ENTITY_SPRITES;
using rtype::game::testing::shippedAssetFolder;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::UNNAMED_SPRITE_SHEET;
using rtype::game::testing::WEAVER_SHEET_FILE;
using rtype::identifiers::ContentFile;

/**
 * Given two sheets naming one sprite sheet, and a sprite sheet no sheet names
 * When the content of their folder is listed
 * Then it holds the two sheets and the named sprite sheet once, by path
 */
TEST(ContentFiles, ListsTheSheetsAndTheSpriteSheetsTheyName) {
  const TemporaryFolder assets;
  assets.write(WEAVER_SHEET_FILE, SHEET_NAMING_THE_BYDOS);
  assets.write(DRIFTER_SHEET_FILE, SHEET_NAMING_THE_BYDOS);
  assets.write(SHARED_SPRITE_SHEET, BYDO_PIXELS);
  assets.write(UNNAMED_SPRITE_SHEET, HUD_PIXELS);

  const std::vector<ContentFile> files =
      contentFilesIn(std::array{assets.path()});

  ASSERT_EQ(files.size(), FILES_OF_TWO_SHEETS_AND_ONE_IMAGE);
  EXPECT_EQ(files[0].path, DRIFTER_SHEET_FILE);
  EXPECT_EQ(files[1].path, WEAVER_SHEET_FILE);
  EXPECT_EQ(files[2].path, SHARED_SPRITE_SHEET);
}

/**
 * Given a sheet and the sprite sheet it names
 * When the content of their folder is listed
 * Then each file carries every byte the folder holds for it
 */
TEST(ContentFiles, CarriesTheBytesOfEachFile) {
  const TemporaryFolder assets;
  assets.write(DRIFTER_SHEET_FILE, SHEET_NAMING_THE_BYDOS);
  assets.write(SHARED_SPRITE_SHEET, BYDO_PIXELS);

  const std::vector<ContentFile> files =
      contentFilesIn(std::array{assets.path()});

  ASSERT_EQ(files.size(), FILES_OF_ONE_SHEET_AND_ITS_IMAGE);
  EXPECT_EQ(files[0].bytes, SHEET_NAMING_THE_BYDOS);
  EXPECT_EQ(files[1].bytes, BYDO_PIXELS);
}

/**
 * Given a sheet naming a sprite sheet whose path comes before the sheets
 * When the content of their folder is listed
 * Then the sprite sheet comes first: the files are in the order of their
 * paths, whatever kind they are
 */
TEST(ContentFiles, OrdersTheFilesByPath) {
  const TemporaryFolder assets;
  assets.write(DRIFTER_SHEET_FILE, SHEET_NAMING_THE_ART);
  assets.write(ART_SPRITE_SHEET, BYDO_PIXELS);

  const std::vector<ContentFile> files =
      contentFilesIn(std::array{assets.path()});

  ASSERT_EQ(files.size(), FILES_OF_ONE_SHEET_AND_ITS_IMAGE);
  EXPECT_EQ(files[0].path, ART_SPRITE_SHEET);
  EXPECT_EQ(files[1].path, DRIFTER_SHEET_FILE);
}

/**
 * Given a sheet naming a sprite sheet the folder does not hold
 * When the content of the folder is listed
 * Then the listing fails, naming the missing file
 */
TEST(ContentFiles, RefusesASpriteSheetTheFolderLacks) {
  const TemporaryFolder assets;
  assets.write(DRIFTER_SHEET_FILE, SHEET_NAMING_A_MISSING_IMAGE);

  EXPECT_THROW(static_cast<void>(contentFilesIn(std::array{assets.path()})),
               UnreadableDataFileException);
}

/**
 * Given a sheet whose appearance names no sprite sheet
 * When the content of its folder is listed
 * Then the listing fails on the missing key
 */
TEST(ContentFiles, RefusesASheetWithoutSpriteSheet) {
  const TemporaryFolder assets;
  assets.write(DRIFTER_SHEET_FILE, SHEET_WITHOUT_SPRITE_SHEET);

  EXPECT_THROW(static_cast<void>(contentFilesIn(std::array{assets.path()})),
               MissingDataFileKeyException);
}

/**
 * Given the assets the game ships with
 * When their content is listed
 * Then every shipped sheet is in it, with the sprite sheets they name, the
 * sheet of the Bydomancer once though three sheets name it
 */
TEST(ContentFiles, ListsEveryShippedSheetAndItsSprites) {
  const std::vector<ContentFile> files =
      contentFilesIn(std::array{shippedAssetFolder()});

  const auto listed = [&files](const std::string& path) {
    return std::ranges::count(files, path, &ContentFile::path);
  };
  for (const EntitySheet& sheet : shippedEntitySheets().all()) {
    EXPECT_EQ(listed(std::string{SHEET_FOLDER_PREFIX} + sheet.name +
                     std::string{SHEET_FILE_EXTENSION}),
              LISTED_ONCE)
        << sheet.name;
  }
  EXPECT_EQ(listed(std::string{SHIPPED_ENTITY_SPRITES}), LISTED_ONCE);
  EXPECT_EQ(listed(std::string{SHIPPED_BYDOMANCER_SPRITES}), LISTED_ONCE);
  EXPECT_TRUE(std::ranges::is_sorted(files, std::less{}, &ContentFile::path));
}

/**
 * Given assets holding a sheet and its sprite sheet, and a resource holding a
 * sheet that names that sprite sheet and a sheet that brings its own
 * When the content of both folders is listed
 * Then it holds the three sheets and both sprite sheets, each read from the
 * folder that holds it
 */
TEST(ContentFiles, LaysTheSheetsOfAResourceOverTheAssets) {
  const TemporaryFolder root;
  const std::array folders{root.path() / ASSETS_OF_THE_STACK,
                           root.path() / RESOURCE_OF_THE_STACK};
  root.write(folders[0] / WEAVER_SHEET_FILE, SHEET_NAMING_THE_BYDOS);
  root.write(folders[0] / SHARED_SPRITE_SHEET, BYDO_PIXELS);
  root.write(folders[1] / DRIFTER_SHEET_FILE, SHEET_NAMING_THE_BYDOS);
  root.write(folders[1] / CRIMSON_SHEET_FILE, SHEET_NAMING_THE_CRIMSON_BYDOS);
  root.write(folders[1] / CRIMSON_SPRITE_SHEET, CRIMSON_PIXELS);

  const std::vector<ContentFile> files = contentFilesIn(folders);

  ASSERT_EQ(files.size(), FILES_OF_THE_STACK);
  EXPECT_EQ(files[0].path, CRIMSON_SHEET_FILE);
  EXPECT_EQ(files[1].path, DRIFTER_SHEET_FILE);
  EXPECT_EQ(files[2].path, WEAVER_SHEET_FILE);
  EXPECT_EQ(files[3].path, SHARED_SPRITE_SHEET);
  EXPECT_EQ(files[3].bytes, BYDO_PIXELS);
  EXPECT_EQ(files[4].path, CRIMSON_SPRITE_SHEET);
  EXPECT_EQ(files[4].bytes, CRIMSON_PIXELS);
}

/**
 * Given assets and a resource that both hold a sheet of the same name
 * When the content of both folders is listed
 * Then the listing is refused for the name
 */
TEST(ContentFiles, RefusesASheetTwoFoldersHold) {
  const TemporaryFolder root;
  const std::array folders{root.path() / ASSETS_OF_THE_STACK,
                           root.path() / RESOURCE_OF_THE_STACK};
  root.write(folders[0] / WEAVER_SHEET_FILE, SHEET_NAMING_THE_BYDOS);
  root.write(folders[0] / SHARED_SPRITE_SHEET, BYDO_PIXELS);
  root.write(folders[1] / WEAVER_SHEET_FILE, SHEET_NAMING_THE_BYDOS);

  EXPECT_THROW(static_cast<void>(contentFilesIn(folders)),
               DuplicateEntityTypeException);
}

/**
 * Given assets and a resource that both hold the sprite sheet a sheet names
 * When the content of both folders is listed
 * Then the listing is refused for that sprite sheet
 */
TEST(ContentFiles, RefusesASpriteSheetTwoFoldersHold) {
  const TemporaryFolder root;
  const std::array folders{root.path() / ASSETS_OF_THE_STACK,
                           root.path() / RESOURCE_OF_THE_STACK};
  root.write(folders[0] / WEAVER_SHEET_FILE, SHEET_NAMING_THE_BYDOS);
  root.write(folders[0] / SHARED_SPRITE_SHEET, BYDO_PIXELS);
  root.write(folders[1] / SHARED_SPRITE_SHEET, CRIMSON_PIXELS);

  EXPECT_THROW(static_cast<void>(contentFilesIn(folders)),
               SharedContentFileException);
}
