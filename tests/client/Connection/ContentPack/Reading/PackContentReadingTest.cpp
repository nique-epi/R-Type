#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <string>
#include "ClientException.hpp"
#include "ContentPackTestFiles.hpp"
#include "ContentPackTestFilesConstants.hpp"
#include "EntityAppearance.hpp"
#include "EntityType.hpp"
#include "PackContent.hpp"
#include "PackContentReading.hpp"
#include "PackFolders.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::EntityAppearance;
using rtype::client::ForeignSpriteSheetException;
using rtype::client::MissingBackgroundFileException;
using rtype::client::PackContent;
using rtype::client::packContentOf;
using rtype::client::PackFolders;
using rtype::client::testing::ASSETS_BELOW_A_PACK;
using rtype::client::testing::BACKGROUND_NAMING_A_MISSING_IMAGE;
using rtype::client::testing::BACKGROUND_SHOWING_THE_LANCERS;
using rtype::client::testing::FOLDER_OF_A_PACK;
using rtype::client::testing::LAIR_NAME;
using rtype::client::testing::LANCER_MASK_PATH;
using rtype::client::testing::LANCER_PIXELS;
using rtype::client::testing::LANCER_SPRITES_PATH;
using rtype::client::testing::LANCER_TYPE;
using rtype::client::testing::lancerFiles;
using rtype::client::testing::lancerFilesNamingAFileOutside;
using rtype::client::testing::lancerFilesWithABackground;
using rtype::client::testing::lancerFilesWithAMask;
using rtype::client::testing::ONE_TYPE;
using rtype::game::EntityType;
using ::testing::ElementsAre;

/**
 * Given the pack of a Bydo this client has no sheet for
 * When it is read
 * Then the Bydo is the first type, an enemy, with its appearance
 */
TEST(PackContentReading, GivesTheTypesOfThePack) {
  const TemporaryFolder folder;

  const PackContent content = packContentOf(
      lancerFiles(), PackFolders{.assets = {}, .pack = folder.path()});

  ASSERT_EQ(content.types.size(), ONE_TYPE);
  EXPECT_EQ(content.types[0].name, LANCER_TYPE);
  EXPECT_EQ(content.types[0].side, EntityType::Enemy);
  EXPECT_NE(content.appearances.named(LANCER_TYPE), nullptr);
}

/**
 * Given the pack of a Bydo and its sprite sheet
 * When it is read
 * Then its sprite sheet is written below the folder, and its appearance names
 * that file by its whole path, so it never takes a local sheet's texture
 */
TEST(PackContentReading, WritesTheSpriteSheetWhereTheAppearanceFindsIt) {
  const TemporaryFolder folder;

  const PackContent content = packContentOf(
      lancerFiles(), PackFolders{.assets = {}, .pack = folder.path()});

  const std::filesystem::path written = folder.path() / LANCER_SPRITES_PATH;
  std::ifstream stream{written, std::ios::binary};
  const std::string pixels{std::istreambuf_iterator<char>{stream},
                           std::istreambuf_iterator<char>{}};
  EXPECT_EQ(pixels, LANCER_PIXELS);
  const EntityAppearance* const lancer = content.appearances.named(LANCER_TYPE);
  ASSERT_NE(lancer, nullptr);
  EXPECT_EQ(lancer->sheet, written.generic_string());
  EXPECT_TRUE(std::filesystem::path{lancer->sheet}.is_absolute());
}

/**
 * Given the pack of a Bydo, its sprite sheet and its mask file
 * When it is read
 * Then only its sprite sheet is written below the folder: a mask file is read
 * with its sheet, and is no sprite sheet an appearance may name
 */
TEST(PackContentReading, WritesNoMaskFileAmongTheSpriteSheets) {
  const TemporaryFolder folder;

  const PackContent content = packContentOf(
      lancerFilesWithAMask(), PackFolders{.assets = {}, .pack = folder.path()});

  EXPECT_EQ(content.types.size(), ONE_TYPE);
  EXPECT_TRUE(std::filesystem::exists(folder.path() / LANCER_SPRITES_PATH));
  EXPECT_FALSE(std::filesystem::exists(folder.path() / LANCER_MASK_PATH));
}

/**
 * Given the pack of a Bydo whose sheet names a sprite sheet outside the pack
 * When it is read
 * Then it is refused before any file is written, so a pack never makes the
 * client read a file of its own
 */
TEST(PackContentReading, RefusesASpriteSheetOutsideThePack) {
  const TemporaryFolder folder;

  EXPECT_THROW(static_cast<void>(packContentOf(
                   lancerFilesNamingAFileOutside(),
                   PackFolders{.assets = {}, .pack = folder.path()})),
               ForeignSpriteSheetException);
  EXPECT_FALSE(std::filesystem::exists(folder.path() / LANCER_SPRITES_PATH));
}

/**
 * Given the pack of a Bydo, which brings no background
 * When it is read
 * Then it gives no backgrounds
 */
TEST(PackContentReading, GivesNoBackgroundsForAPackWithoutOne) {
  const TemporaryFolder folder;

  const PackContent content = packContentOf(
      lancerFiles(), PackFolders{.assets = {}, .pack = folder.path()});

  EXPECT_EQ(content.backgrounds, nullptr);
}

/**
 * Given a pack that brings a background showing its sprite sheet
 * When it is read
 * Then the background is found by its name
 */
TEST(PackContentReading, GivesTheBackgroundsOfThePack) {
  const TemporaryFolder folder;
  const PackFolders folders{.assets = folder.path() / ASSETS_BELOW_A_PACK,
                            .pack = folder.path() / FOLDER_OF_A_PACK};

  const PackContent content = packContentOf(
      lancerFilesWithABackground(BACKGROUND_SHOWING_THE_LANCERS), folders);

  ASSERT_NE(content.backgrounds, nullptr);
  EXPECT_NE(content.backgrounds->named(LAIR_NAME), nullptr);
}

/**
 * Given a pack whose background shows the sprite sheet the pack brings
 * When it is read
 * Then the background names that image by the whole path the pack wrote it
 * at, so it never takes a local image's texture
 */
TEST(PackContentReading, ABackgroundNamesAnImageOfThePackByItsWholePath) {
  const TemporaryFolder folder;
  const PackFolders folders{.assets = folder.path() / ASSETS_BELOW_A_PACK,
                            .pack = folder.path() / FOLDER_OF_A_PACK};

  const PackContent content = packContentOf(
      lancerFilesWithABackground(BACKGROUND_SHOWING_THE_LANCERS), folders);

  ASSERT_NE(content.backgrounds, nullptr);
  EXPECT_THAT(
      content.backgrounds->images(),
      ElementsAre(std::filesystem::absolute(folders.pack / LANCER_SPRITES_PATH)
                      .generic_string()));
}

/**
 * Given a pack whose background names an image neither the pack nor the
 * assets hold
 * When it is read
 * Then it is refused for that image
 */
TEST(PackContentReading, RefusesABackgroundNamingAFileItCannotFind) {
  const TemporaryFolder folder;
  const PackFolders folders{.assets = folder.path() / ASSETS_BELOW_A_PACK,
                            .pack = folder.path() / FOLDER_OF_A_PACK};

  EXPECT_THROW(
      static_cast<void>(packContentOf(
          lancerFilesWithABackground(BACKGROUND_NAMING_A_MISSING_IMAGE),
          folders)),
      MissingBackgroundFileException);
}
