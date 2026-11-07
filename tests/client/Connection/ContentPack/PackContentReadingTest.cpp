#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include "ClientException.hpp"
#include "ContentPackTestFiles.hpp"
#include "ContentPackTestFilesConstants.hpp"
#include "EntityAppearance.hpp"
#include "EntityType.hpp"
#include "PackContent.hpp"
#include "PackContentReading.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::EntityAppearance;
using rtype::client::ForeignSpriteSheetException;
using rtype::client::PackContent;
using rtype::client::packContentOf;
using rtype::client::testing::LANCER_PIXELS;
using rtype::client::testing::LANCER_SPRITES_PATH;
using rtype::client::testing::LANCER_TYPE;
using rtype::client::testing::lancerFiles;
using rtype::client::testing::lancerFilesNamingAFileOutside;
using rtype::client::testing::ONE_TYPE;
using rtype::game::EntityType;

/**
 * Given the pack of a Bydo this client has no sheet for
 * When it is read
 * Then the Bydo is the first type, an enemy, with its appearance
 */
TEST(PackContentReading, GivesTheTypesOfThePack) {
  const TemporaryFolder folder;

  const PackContent content = packContentOf(lancerFiles(), folder.path());

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

  const PackContent content = packContentOf(lancerFiles(), folder.path());

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
 * Given the pack of a Bydo whose sheet names a sprite sheet outside the pack
 * When it is read
 * Then it is refused before any file is written, so a pack never makes the
 * client read a file of its own
 */
TEST(PackContentReading, RefusesASpriteSheetOutsideThePack) {
  const TemporaryFolder folder;

  EXPECT_THROW(static_cast<void>(packContentOf(lancerFilesNamingAFileOutside(),
                                               folder.path())),
               ForeignSpriteSheetException);
  EXPECT_FALSE(std::filesystem::exists(folder.path() / LANCER_SPRITES_PATH));
}
