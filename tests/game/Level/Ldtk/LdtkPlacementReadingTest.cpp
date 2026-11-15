#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "EngineException.hpp"
#include "GameException.hpp"
#include "LdtkConstants.hpp"
#include "LdtkPlacement.hpp"
#include "LdtkPlacementFixtures.hpp"
#include "LdtkPlacementReading.hpp"
#include "LdtkPlacementReadingTestConstants.hpp"
#include "LdtkPlacementTestConstants.hpp"
#include "TemporaryFolder.hpp"

using rtype::engine::UnreadableDataFileException;
using rtype::game::InvalidLdtkFileException;
using rtype::game::LdtkPlacement;
using rtype::game::ldtkPlacementsOf;
using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

/**
 * Given a level whose Mobs layer lists a Lamprey, then a BydoViolet further
 * left
 * When its placements are read
 * Then both come, the BydoViolet first, in the order of their x
 */
TEST(LdtkPlacementReading, ReadsTheEnemiesOfTheMobsLayerSortedByX) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, oneLevelProject(LAMPREY_THEN_BYDO_VIOLET));

  const std::vector<LdtkPlacement> placements =
      ldtkPlacementsOf(root.path() / TEST_LDTK_FILE);

  ASSERT_EQ(placements.size(), TWO_ENEMIES);
  EXPECT_EQ(placements[0].sheet, BYDO_VIOLET_SHEET);
  EXPECT_EQ(placements[1].sheet, LAMPREY_SHEET);
}

/**
 * Given a Lamprey of 32 by 40 pixels placed at pixel 368, 32 with its pivot
 * at its top left
 * When its placement is read
 * Then its center is 1536, 208, half its box in, at 4 units a pixel, and its
 * width 128
 */
TEST(LdtkPlacementReading, ConvertsThePivotOfAnEntityToItsCenter) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, oneLevelProject(LAMPREY_THEN_BYDO_VIOLET));

  const std::vector<LdtkPlacement> placements =
      ldtkPlacementsOf(root.path() / TEST_LDTK_FILE);

  ASSERT_EQ(placements.size(), TWO_ENEMIES);
  EXPECT_EQ(placements[1].entity, LAMPREY_ENTITY);
  EXPECT_FLOAT_EQ(placements[1].position.x, LAMPREY_CENTER_X);
  EXPECT_FLOAT_EQ(placements[1].position.y, LAMPREY_CENTER_Y);
  EXPECT_FLOAT_EQ(placements[1].width, LAMPREY_WIDTH);
}

/**
 * Given a Lamprey placed at the top left corner of its level
 * When its placement is read
 * Then its center is 64, 80, half its box from each edge
 */
TEST(LdtkPlacementReading, ConvertsAnEntityAtTheLeftEdge) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, oneLevelProject(LAMPREY_AT_THE_TOP_LEFT));

  const std::vector<LdtkPlacement> placements =
      ldtkPlacementsOf(root.path() / TEST_LDTK_FILE);

  ASSERT_EQ(placements.size(), 1);
  EXPECT_FLOAT_EQ(placements[0].position.x, TOP_LEFT_CENTER_X);
  EXPECT_FLOAT_EQ(placements[0].position.y, TOP_LEFT_CENTER_Y);
}

/**
 * Given a Lamprey placed against the right and bottom edges of a 480 by 270
 * level
 * When its placement is read
 * Then its center is 1856, 1000, half its box from each edge
 */
TEST(LdtkPlacementReading, ConvertsAnEntityAtTheRightAndBottomEdges) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, oneLevelProject(LAMPREY_AT_THE_BOTTOM_RIGHT));

  const std::vector<LdtkPlacement> placements =
      ldtkPlacementsOf(root.path() / TEST_LDTK_FILE);

  ASSERT_EQ(placements.size(), 1);
  EXPECT_FLOAT_EQ(placements[0].position.x, BOTTOM_RIGHT_CENTER_X);
  EXPECT_FLOAT_EQ(placements[0].position.y, BOTTOM_RIGHT_CENTER_Y);
}

/**
 * Given a Lamprey whose pivot is its center, placed at pixel 100, 100
 * When its placement is read
 * Then its center is that pixel, 400, 400
 */
TEST(LdtkPlacementReading, ReadsACenteredPivotWhereItIs) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, oneLevelProject(LAMPREY_WITH_A_CENTERED_PIVOT));

  const std::vector<LdtkPlacement> placements =
      ldtkPlacementsOf(root.path() / TEST_LDTK_FILE);

  ASSERT_EQ(placements.size(), 1);
  EXPECT_FLOAT_EQ(placements[0].position.x, CENTERED_PIVOT_CENTER);
  EXPECT_FLOAT_EQ(placements[0].position.y, CENTERED_PIVOT_CENTER);
}

/**
 * Given two levels of 480 pixels, the second placing a Lamprey at its top
 * left corner
 * When the placements are read
 * Then the Lamprey is 1920 units in, after the first level, plus half its box
 */
TEST(LdtkPlacementReading, LaysTheSecondLevelAfterTheFirst) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, TWO_LEVELS_PROJECT);

  const std::vector<LdtkPlacement> placements =
      ldtkPlacementsOf(root.path() / TEST_LDTK_FILE);

  ASSERT_EQ(placements.size(), 1);
  EXPECT_FLOAT_EQ(placements[0].position.x, SECOND_LEVEL_CENTER_X);
}

/**
 * Given an entity named BydoViolet
 * When its placement is read
 * Then it keeps that identifier, and names the sheet bydo_violet
 */
TEST(LdtkPlacementReading, NamesTheSheetAfterTheIdentifier) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, oneLevelProject(LAMPREY_THEN_BYDO_VIOLET));

  const std::vector<LdtkPlacement> placements =
      ldtkPlacementsOf(root.path() / TEST_LDTK_FILE);

  ASSERT_EQ(placements.size(), TWO_ENEMIES);
  EXPECT_EQ(placements[0].entity, BYDO_VIOLET_ENTITY);
  EXPECT_EQ(placements[0].sheet, BYDO_VIOLET_SHEET);
}

/**
 * Given an entity with an Int, a Float, a Bool, a String and an Enum field,
 * and a field whose value is null
 * When its placement is read
 * Then the five set fields come typed, in the order of the file, and the
 * null one is left out
 */
TEST(LdtkPlacementReading, ReadsTheScalarFieldsAndLeavesOutANullOne) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, oneLevelProject(LAMPREY_WITH_FIELDS));

  const std::vector<LdtkPlacement> placements =
      ldtkPlacementsOf(root.path() / TEST_LDTK_FILE);

  ASSERT_EQ(placements.size(), 1);
  ASSERT_EQ(placements[0].fields.size(), FIELDS_READ);
  EXPECT_EQ(placements[0].fields[0].name, COUNT_FIELD_NAME);
  EXPECT_EQ(std::get<double>(placements[0].fields[0].value), COUNT_FIELD);
  EXPECT_EQ(std::get<double>(placements[0].fields[1].value), SPEED_FIELD);
  EXPECT_TRUE(std::get<bool>(placements[0].fields[2].value));
  EXPECT_EQ(std::get<std::string>(placements[0].fields[3].value), NAME_FIELD);
  EXPECT_EQ(placements[0].fields[4].name, SIDE_FIELD_NAME);
  EXPECT_EQ(std::get<std::string>(placements[0].fields[4].value), SIDE_FIELD);
}

/**
 * Given an entity with a field of a kind the game does not read, a point
 * When its placement is read
 * Then the file is refused, the message naming the field
 */
TEST(LdtkPlacementReading, RefusesAFieldOfAnotherKind) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, oneLevelProject(LAMPREY_WITH_A_POINT_FIELD));

  EXPECT_THAT(
      [&] {
        static_cast<void>(ldtkPlacementsOf(root.path() / TEST_LDTK_FILE));
      },
      ThrowsMessage<InvalidLdtkFileException>(
          HasSubstr(std::string{rtype::game::UNSUPPORTED_FIELD_REASON} +
                    std::string{POINT_FIELD_NAME})));
}

/**
 * Given an entity whose identifier is 25 letters long
 * When its placement is read
 * Then the file is refused, since no sheet can have that name
 */
TEST(LdtkPlacementReading, RefusesAnIdentifierThatIsNoSheetName) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, oneLevelProject(ENTITY_WITH_A_LONG_IDENTIFIER));

  EXPECT_THAT(
      [&] {
        static_cast<void>(ldtkPlacementsOf(root.path() / TEST_LDTK_FILE));
      },
      ThrowsMessage<InvalidLdtkFileException>(
          HasSubstr(std::string{rtype::game::NOT_A_SHEET_NAME_REASON} +
                    std::string{LONG_IDENTIFIER})));
}

/**
 * Given a level without a Mobs layer of entities
 * When its placements are read
 * Then the file is refused, the message naming the level
 */
TEST(LdtkPlacementReading, RefusesALevelWithoutMobsLayer) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, NO_MOBS_LAYER_PROJECT);

  EXPECT_THAT(
      [&] {
        static_cast<void>(ldtkPlacementsOf(root.path() / TEST_LDTK_FILE));
      },
      ThrowsMessage<InvalidLdtkFileException>(
          HasSubstr(std::string{rtype::game::NO_MOBS_LAYER_REASON} +
                    std::string{FIRST_LEVEL})));
}

/**
 * Given a project whose levels are saved in separate files
 * When its placements are read
 * Then the file is refused, asking to turn the option off
 */
TEST(LdtkPlacementReading, RefusesLevelsSavedSeparately) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, SEPARATE_LEVELS_PROJECT);

  EXPECT_THAT(
      [&] {
        static_cast<void>(ldtkPlacementsOf(root.path() / TEST_LDTK_FILE));
      },
      ThrowsMessage<InvalidLdtkFileException>(
          HasSubstr(rtype::game::LEVELS_SAVED_SEPARATELY_REASON)));
}

/**
 * Given a file that is no JSON
 * When it is read as a project
 * Then the file is refused, the message naming the file
 */
TEST(LdtkPlacementReading, RefusesATextThatIsNotJson) {
  const TemporaryFolder root;
  root.write(TEST_LDTK_FILE, NOT_JSON);

  EXPECT_THAT(
      [&] {
        static_cast<void>(ldtkPlacementsOf(root.path() / TEST_LDTK_FILE));
      },
      ThrowsMessage<InvalidLdtkFileException>(::testing::AllOf(
          HasSubstr(rtype::game::NOT_JSON_REASON), HasSubstr(TEST_LDTK_FILE))));
}

/**
 * Given a file no folder holds
 * When it is read as a project
 * Then it is refused as unreadable, the message naming it
 */
TEST(LdtkPlacementReading, RefusesAMissingFile) {
  const TemporaryFolder root;

  EXPECT_THAT(
      [&] {
        static_cast<void>(ldtkPlacementsOf(root.path() / TEST_LDTK_FILE));
      },
      ThrowsMessage<UnreadableDataFileException>(HasSubstr(TEST_LDTK_FILE)));
}
