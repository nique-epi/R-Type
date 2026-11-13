#include <gtest/gtest.h>
#include <cstddef>
#include <span>
#include <vector>
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetsFixtures.hpp"
#include "EntitySheetsTestConstants.hpp"
#include "EntityType.hpp"
#include "GameException.hpp"
#include "NumberedEntityType.hpp"
#include "NumberedEntityTypes.hpp"
#include "NumberedEntityTypesTestConstants.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"
#include "SheetTextReading.hpp"
#include "ShippedEntitySheets.hpp"

using rtype::engine::MissingDataFileKeyException;
using rtype::game::EntitySheet;
using rtype::game::EntityType;
using rtype::game::NumberedEntityType;
using rtype::game::numberedEntityTypesOf;
using rtype::game::SheetText;
using rtype::game::sheetTextsIn;
using rtype::game::TooManyEntityTypesException;
using rtype::game::testing::enemyMissileSheets;
using rtype::game::testing::LANCER_BOX_HEIGHT;
using rtype::game::testing::LANCER_BOX_WIDTH;
using rtype::game::testing::LANCER_TYPE;
using rtype::game::testing::MOST_SHEETS_LOADED;
using rtype::game::testing::ONE_SHEET_TOO_MANY;
using rtype::game::testing::ONE_TYPE;
using rtype::game::testing::SECOND_LISTED_BOX_WIDTH;
using rtype::game::testing::SHEET_OF_A_PLUGIN_PILOT;
using rtype::game::testing::SHEET_OF_TWO_BOXES;
using rtype::game::testing::SHEET_WITHOUT_BOX;
using rtype::game::testing::SHEET_WITHOUT_SIDE;
using rtype::game::testing::sheetText;
using rtype::game::testing::SHIP_TEXT;
using rtype::game::testing::SHIP_TYPE;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::shippedSheetFolder;
using rtype::game::testing::TWO_LISTED_BOXES;
using rtype::game::testing::TWO_TYPES;
using rtype::game::testing::WEAVER_TEXT;
using rtype::game::testing::WEAVER_TYPE;

/**
 * Given the sheets the game ships with
 * When a client numbers their types
 * Then each type has the number, the name, the side and the box the server
 * gives it
 */
TEST(NumberedEntityTypes, NumbersTheShippedTypesAsTheServerDoes) {
  const std::vector<NumberedEntityType> types =
      numberedEntityTypesOf(sheetTextsIn(shippedSheetFolder()));

  const std::span<const EntitySheet> sheets = shippedEntitySheets().all();
  ASSERT_EQ(types.size(), sheets.size());
  for (std::size_t index = 0; index < sheets.size(); ++index) {
    SCOPED_TRACE(sheets[index].name);
    EXPECT_EQ(types[index].name, sheets[index].name);
    EXPECT_EQ(types[index].side, sheets[index].side);
    EXPECT_FLOAT_EQ(types[index].hitbox.width, sheets[index].hitbox.width);
    EXPECT_FLOAT_EQ(types[index].hitbox.height, sheets[index].hitbox.height);
  }
}

/**
 * Given two sheets out of the order of their names
 * When their types are numbered
 * Then the first name comes first, each with its side
 */
TEST(NumberedEntityTypes, NumbersTheTypesInTheOrderOfTheirNames) {
  const std::vector<SheetText> sheets{sheetText(WEAVER_TYPE, WEAVER_TEXT),
                                      sheetText(SHIP_TYPE, SHIP_TEXT)};

  const std::vector<NumberedEntityType> types = numberedEntityTypesOf(sheets);

  ASSERT_EQ(types.size(), TWO_TYPES);
  EXPECT_EQ(types[0].name, SHIP_TYPE);
  EXPECT_EQ(types[0].side, EntityType::Player);
  EXPECT_EQ(types[1].name, WEAVER_TYPE);
  EXPECT_EQ(types[1].side, EntityType::Enemy);
}

/**
 * Given a sheet whose pilot is a behaviour only a plugin of the server knows
 * When its type is numbered by a client that knows no such behaviour
 * Then it is numbered with its side and its box: only those are read
 */
TEST(NumberedEntityTypes, ReadsTheSideOfASheetWhosePilotItDoesNotKnow) {
  const std::vector<SheetText> sheets{
      sheetText(LANCER_TYPE, SHEET_OF_A_PLUGIN_PILOT)};

  const std::vector<NumberedEntityType> types = numberedEntityTypesOf(sheets);

  ASSERT_EQ(types.size(), ONE_TYPE);
  EXPECT_EQ(types[0].side, EntityType::Enemy);
  EXPECT_FLOAT_EQ(types[0].hitbox.width, LANCER_BOX_WIDTH);
  EXPECT_FLOAT_EQ(types[0].hitbox.height, LANCER_BOX_HEIGHT);
}

/**
 * Given a sheet that names no side
 * When its type is numbered
 * Then the numbering fails on the missing key
 */
TEST(NumberedEntityTypes, RefusesASheetWithoutSide) {
  const std::vector<SheetText> sheets{
      sheetText(LANCER_TYPE, SHEET_WITHOUT_SIDE)};

  EXPECT_THROW(static_cast<void>(numberedEntityTypesOf(sheets)),
               MissingDataFileKeyException);
}

/**
 * Given a sheet that names a side and no box
 * When its type is numbered
 * Then the numbering fails on the missing box, as the server would
 */
TEST(NumberedEntityTypes, RefusesASheetWithoutBox) {
  const std::vector<SheetText> sheets{
      sheetText(LANCER_TYPE, SHEET_WITHOUT_BOX)};

  EXPECT_THROW(static_cast<void>(numberedEntityTypesOf(sheets)),
               MissingDataFileKeyException);
}

/**
 * Given 255 sheets, the most a program loads
 * When a client numbers their types
 * Then all 255 are numbered
 */
TEST(NumberedEntityTypes, NumbersTheMostTypesAProgramLoads) {
  const std::vector<SheetText> sheets = enemyMissileSheets(MOST_SHEETS_LOADED);

  EXPECT_EQ(numberedEntityTypesOf(sheets).size(), MOST_SHEETS_LOADED);
}

/**
 * Given 256 sheets in a pack
 * When a client numbers their types
 * Then they are refused, as the server refuses them
 */
TEST(NumberedEntityTypes, RefusesOneTypePastTheMost) {
  const std::vector<SheetText> sheets = enemyMissileSheets(ONE_SHEET_TOO_MANY);

  EXPECT_THROW(static_cast<void>(numberedEntityTypesOf(sheets)),
               TooManyEntityTypesException);
}

/**
 * Given a sheet whose hitbox lists two boxes
 * When a client numbers its type
 * Then the type keeps those two boxes, which the client tests its missiles
 * against
 */
TEST(NumberedEntityTypes, KeepsTheBoxesAHitboxLists) {
  const std::vector<SheetText> sheets{
      sheetText(LANCER_TYPE, SHEET_OF_TWO_BOXES)};

  const std::vector<NumberedEntityType> types = numberedEntityTypesOf(sheets);

  ASSERT_EQ(types.size(), ONE_TYPE);
  ASSERT_EQ(types[0].hitboxParts.size(), TWO_LISTED_BOXES);
  EXPECT_FLOAT_EQ(types[0].hitboxParts.back().width, SECOND_LISTED_BOX_WIDTH);
}
