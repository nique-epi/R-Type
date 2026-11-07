#include <gtest/gtest.h>
#include <cstddef>
#include <span>
#include <vector>
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntityType.hpp"
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
using rtype::game::testing::LANCER_TYPE;
using rtype::game::testing::ONE_TYPE;
using rtype::game::testing::SHEET_OF_A_PLUGIN_PILOT;
using rtype::game::testing::SHEET_WITHOUT_SIDE;
using rtype::game::testing::sheetText;
using rtype::game::testing::SHIP_TEXT;
using rtype::game::testing::SHIP_TYPE;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::shippedSheetFolder;
using rtype::game::testing::TWO_TYPES;
using rtype::game::testing::WEAVER_TEXT;
using rtype::game::testing::WEAVER_TYPE;

/**
 * Given the sheets the game ships with
 * When a client numbers their types
 * Then each type has the number, the name and the side the server gives it
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
 * Then it is numbered with its side: only the side is read
 */
TEST(NumberedEntityTypes, ReadsTheSideOfASheetWhosePilotItDoesNotKnow) {
  const std::vector<SheetText> sheets{
      sheetText(LANCER_TYPE, SHEET_OF_A_PLUGIN_PILOT)};

  const std::vector<NumberedEntityType> types = numberedEntityTypesOf(sheets);

  ASSERT_EQ(types.size(), ONE_TYPE);
  EXPECT_EQ(types[0].side, EntityType::Enemy);
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
