#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <array>
#include <filesystem>
#include "EngineException.hpp"
#include "GameException.hpp"
#include "LevelPlacementFilesFixture.hpp"
#include "LevelPlacementFilesTestConstants.hpp"
#include "LuauLevelConstants.hpp"
#include "LuauLevelEntry.hpp"
#include "LuauLevelsTestConstants.hpp"
#include "ShippedEntitySheets.hpp"

using rtype::engine::UnreadableDataFileException;
using rtype::game::InvalidLdtkFileException;
using rtype::game::LuauLevelEntry;
using rtype::game::testing::shippedAssetFolder;
using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

/**
 * Given the assets and the resource training-run the repository ships
 * When the placements of its levels are read
 * Then the level training_run places the 15 enemies of its LDtk file
 */
TEST_F(LevelPlacementFilesFixture, ReadsThePlacementsOfTheShippedLevel) {
  const std::array folders{
      shippedAssetFolder(),
      std::filesystem::path{RTYPE_SOURCE_RESOURCES_FOLDER} /
          SHIPPED_LEVEL_RESOURCE};

  readPlacements(folders);

  const LuauLevelEntry* const entry = levels().entryOf(SHIPPED_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->placements.size(), ENEMIES_OF_THE_SHIPPED_LEVEL);
}

/**
 * Given a content folder whose level places an entity Ghost, which names no
 * sheet
 * When its placements are read
 * Then the file is refused, the message naming the entity and where it is
 */
TEST_F(LevelPlacementFilesFixture, RefusesAnEntityThatNamesNoSheet) {
  writeContent(FIRST_CONTENT, GHOST_AT_THE_CORNER);
  const std::array folders{contentFolder(FIRST_CONTENT)};

  EXPECT_THAT(
      [&] { readPlacements(folders); },
      ThrowsMessage<InvalidLdtkFileException>(HasSubstr(GHOST_REFUSAL)));
}

/**
 * Given a content folder whose level places a Ship
 * When its placements are read
 * Then the file is refused, saying no level may place a ship
 */
TEST_F(LevelPlacementFilesFixture, RefusesAnEntityThatNamesAShip) {
  writeContent(FIRST_CONTENT, SHIP_AT_THE_CORNER);
  const std::array folders{contentFolder(FIRST_CONTENT)};

  EXPECT_THAT([&] { readPlacements(folders); },
              ThrowsMessage<InvalidLdtkFileException>(
                  HasSubstr(rtype::game::SHIP_SHEET_REASON)));
}

/**
 * Given a content folder whose level names an LDtk file no folder holds
 * When its placements are read
 * Then the file is refused as unreadable, the message naming it
 */
TEST_F(LevelPlacementFilesFixture, RefusesAMissingFile) {
  writeContent(FIRST_CONTENT, DRIFTER_AT_THE_CORNER);
  std::filesystem::remove(contentFolder(FIRST_CONTENT) / PLACED_LEVEL_LDTK);
  const std::array folders{contentFolder(FIRST_CONTENT)};

  EXPECT_THAT(
      [&] { readPlacements(folders); },
      ThrowsMessage<UnreadableDataFileException>(HasSubstr(PLACED_LEVEL_LDTK)));
}

/**
 * Given two content folders that both hold the LDtk file of the level, the
 * Drifter of the second one further right
 * When its placements are read
 * Then the placement comes from the second folder, the one laid last
 */
TEST_F(LevelPlacementFilesFixture, TheContentFolderLaidLastWins) {
  writeContent(FIRST_CONTENT, DRIFTER_AT_THE_CORNER);
  writeContent(SECOND_CONTENT, DRIFTER_FURTHER_RIGHT);
  std::filesystem::remove(contentFolder(SECOND_CONTENT) / PLACED_LEVEL_SCRIPT);
  const std::array folders{contentFolder(FIRST_CONTENT),
                           contentFolder(SECOND_CONTENT)};

  readPlacements(folders);

  const LuauLevelEntry* const entry = levels().entryOf(PLACED_LEVEL);
  ASSERT_NE(entry, nullptr);
  ASSERT_EQ(entry->placements.size(), 1);
  EXPECT_FLOAT_EQ(entry->placements[0].position.x, FURTHER_RIGHT_CENTER_X);
  EXPECT_FLOAT_EQ(entry->placements[0].position.y, CORNER_CENTER);
}
