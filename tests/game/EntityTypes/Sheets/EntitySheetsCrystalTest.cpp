#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "DataFileText.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySheetsCrystalTestConstants.hpp"
#include "EntitySheetsTestConstants.hpp"
#include "EntitySheetsWeaponTestConstants.hpp"
#include "GameException.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"

using rtype::game::EntitySheet;
using rtype::game::EntitySheets;
using rtype::game::InconsistentEntitySheetException;
using rtype::game::SheetText;
using rtype::game::testing::ARMED_SHIP_NAME;
using rtype::game::testing::ARMED_SHIP_TEXT;
using rtype::game::testing::CARRIER_NAME;
using rtype::game::testing::CARRIER_TEXT;
using rtype::game::testing::DROPPING_MISSILE_NAME;
using rtype::game::testing::DROPPING_MISSILE_TEXT;
using rtype::game::testing::FAN_SHOT_NAME;
using rtype::game::testing::FAN_SHOT_TEXT;
using rtype::game::testing::GREEN_CRYSTAL_TEXT;
using rtype::game::testing::MISSILE_CARRIER_TEXT;
using rtype::game::testing::PLAYER_MISSILE_NAME_IN_TESTS;
using rtype::game::testing::PLAYER_MISSILE_SHEET_TEXT;
using rtype::game::testing::RED_CRYSTAL_NAME;
using rtype::game::testing::RED_CRYSTAL_TEXT;
using rtype::game::testing::RED_WEAPON;
using rtype::game::testing::RIGHTWARD_CRYSTAL_TEXT;
using rtype::game::testing::sheetText;

/**
 * Given the armed ship and its missiles, a red crystal, and a Bydo that drops
 * it
 * When the sheets are read
 * Then the drop of the Bydo holds the number of the sheet of the crystal
 */
TEST(EntitySheets, NumbersTheCrystalsAnEnemyDrops) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, ARMED_SHIP_TEXT),
      sheetText(FAN_SHOT_NAME, FAN_SHOT_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT),
      sheetText(RED_CRYSTAL_NAME, RED_CRYSTAL_TEXT),
      sheetText(CARRIER_NAME, CARRIER_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  const EntitySheet& carrier = sheets.named(CARRIER_NAME);
  ASSERT_EQ(carrier.drops.size(), 1U);
  EXPECT_EQ(carrier.drops.front().number,
            sheets.named(RED_CRYSTAL_NAME).number);
}

/**
 * Given the armed ship and its missiles, and a red crystal
 * When the sheets are read
 * Then the crystal names the weapon red
 */
TEST(EntitySheets, ReadsTheWeaponOfACrystal) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, ARMED_SHIP_TEXT),
      sheetText(FAN_SHOT_NAME, FAN_SHOT_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT),
      sheetText(RED_CRYSTAL_NAME, RED_CRYSTAL_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  const EntitySheet& crystal = sheets.named(RED_CRYSTAL_NAME);
  ASSERT_TRUE(crystal.crystal.has_value());
  EXPECT_EQ(crystal.crystal.value_or(rtype::engine::DataFileText{}).text,
            std::string{RED_WEAPON});
}

/**
 * Given the armed ship and its missiles, and a crystal of the weapon green
 * that no ship holds
 * When the sheets are read
 * Then they are refused: such a crystal would give nothing
 */
TEST(EntitySheets, RefusesACrystalOfAWeaponNoShipHolds) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, ARMED_SHIP_TEXT),
      sheetText(FAN_SHOT_NAME, FAN_SHOT_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT),
      sheetText(RED_CRYSTAL_NAME, GREEN_CRYSTAL_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}

/**
 * Given the armed ship and its missiles, and a red crystal that drifts right
 * When the sheets are read
 * Then they are refused: a crystal nobody picks up would never leave
 */
TEST(EntitySheets, RefusesACrystalThatDriftsRight) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, ARMED_SHIP_TEXT),
      sheetText(FAN_SHOT_NAME, FAN_SHOT_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT),
      sheetText(RED_CRYSTAL_NAME, RIGHTWARD_CRYSTAL_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}

/**
 * Given a Bydo that drops the missile of a player, and that missile
 * When the sheets are read
 * Then they are refused: an enemy drops crystals only
 */
TEST(EntitySheets, RefusesTheDropOfABodyThatIsNoCrystal) {
  const std::vector<SheetText> texts{
      sheetText(CARRIER_NAME, MISSILE_CARRIER_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}

/**
 * Given the armed ship and its missiles, a red crystal, and a missile of an
 * enemy that drops it
 * When the sheets are read
 * Then they are refused: only an enemy drops crystals
 */
TEST(EntitySheets, RefusesTheDropsOfABodyThatIsNoEnemy) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, ARMED_SHIP_TEXT),
      sheetText(FAN_SHOT_NAME, FAN_SHOT_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT),
      sheetText(RED_CRYSTAL_NAME, RED_CRYSTAL_TEXT),
      sheetText(DROPPING_MISSILE_NAME, DROPPING_MISSILE_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}
