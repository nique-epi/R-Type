#include <gtest/gtest.h>
#include <vector>
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySheetsTestConstants.hpp"
#include "EntitySheetsVolleyTestConstants.hpp"
#include "GameException.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"

using rtype::engine::UnknownDataFileChoiceException;
using rtype::game::EntitySheet;
using rtype::game::EntitySheets;
using rtype::game::InconsistentEntitySheetException;
using rtype::game::SheetText;
using rtype::game::testing::BROODER_NAME_IN_SHEET_TESTS;
using rtype::game::testing::BROODER_OF_MINIONS_TEXT;
using rtype::game::testing::BROODER_OF_PLAYER_MISSILES_TEXT;
using rtype::game::testing::MINION_NAME_IN_SHEET_TESTS;
using rtype::game::testing::MINION_SHEET_TEXT;
using rtype::game::testing::PLAYER_MISSILE_NAME_IN_TESTS;
using rtype::game::testing::PLAYER_MISSILE_SHEET_TEXT;
using rtype::game::testing::sheetText;

/**
 * Given the sheets of a brooder whose volley is made of minions, and of the
 * minion
 * When the sheets are read
 * Then the volley of the brooder holds the number of the sheet of the minion
 */
TEST(EntitySheets, NumbersTheBodyOfAVolley) {
  const std::vector<SheetText> texts{
      sheetText(BROODER_NAME_IN_SHEET_TESTS, BROODER_OF_MINIONS_TEXT),
      sheetText(MINION_NAME_IN_SHEET_TESTS, MINION_SHEET_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  const EntitySheet& brooder = sheets.named(BROODER_NAME_IN_SHEET_TESTS);
  ASSERT_EQ(brooder.volleys.size(), 1U);
  EXPECT_EQ(brooder.volleys.front().bodyNumber,
            sheets.named(MINION_NAME_IN_SHEET_TESTS).number);
}

/**
 * Given the sheet of a brooder whose volley names a sheet that is not read
 * When the sheets are read
 * Then they are refused: the body of a volley is a sheet
 */
TEST(EntitySheets, RefusesAVolleyOfNoSheet) {
  const std::vector<SheetText> texts{
      sheetText(BROODER_NAME_IN_SHEET_TESTS, BROODER_OF_MINIONS_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               UnknownDataFileChoiceException);
}

/**
 * Given the sheet of a brooder whose volley is made of missiles of a player,
 * and of that missile
 * When the sheets are read
 * Then they are refused: a volley is made of enemies or of their missiles
 */
TEST(EntitySheets, RefusesAVolleyOfPlayerMissiles) {
  const std::vector<SheetText> texts{
      sheetText(BROODER_NAME_IN_SHEET_TESTS, BROODER_OF_PLAYER_MISSILES_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}
