#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySheetsTestConstants.hpp"
#include "EntitySheetsWeaponTestConstants.hpp"
#include "GameException.hpp"
#include "GunSheet.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"
#include "ShotPattern.hpp"
#include "WeaponSheet.hpp"

using rtype::game::EntitySheet;
using rtype::game::EntitySheets;
using rtype::game::GunSheet;
using rtype::game::InconsistentEntitySheetException;
using rtype::game::SheetText;
using rtype::game::ShotPattern;
using rtype::game::WeaponSheet;
using rtype::game::testing::ARMED_BYDO_NAME;
using rtype::game::testing::ARMED_BYDO_TEXT;
using rtype::game::testing::ARMED_SHIP_NAME;
using rtype::game::testing::ARMED_SHIP_TEXT;
using rtype::game::testing::BLUE_LEVELS;
using rtype::game::testing::BLUE_WEAPON;
using rtype::game::testing::FAN_SHOT_NAME;
using rtype::game::testing::FAN_SHOT_TEXT;
using rtype::game::testing::FIRST_RED_INTERVAL_TICKS;
using rtype::game::testing::FIRST_RED_PATTERN;
using rtype::game::testing::PLAYER_MISSILE_NAME_IN_TESTS;
using rtype::game::testing::PLAYER_MISSILE_SHEET_TEXT;
using rtype::game::testing::RED_LEVELS;
using rtype::game::testing::RED_WEAPON;
using rtype::game::testing::SCATTER_ON_THE_MUZZLE_SHIP_TEXT;
using rtype::game::testing::sheetText;
using rtype::game::testing::STRAIGHT_FAN_SHIP_TEXT;
using rtype::game::testing::THREE_LEVEL_SHIP_TEXT;
using rtype::game::testing::TWINNED_NAME_SHIP_TEXT;

/**
 * Given the sheets of a ship with a red and a blue weapon, and of their
 * missiles
 * When the sheets are read
 * Then the ship holds red with two levels, then blue with one
 */
TEST(EntitySheets, ReadsTheWeaponsOfAShipInTheOrderOfItsSheet) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, ARMED_SHIP_TEXT),
      sheetText(FAN_SHOT_NAME, FAN_SHOT_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  const std::vector<WeaponSheet>& weapons =
      sheets.named(ARMED_SHIP_NAME).weapons;
  ASSERT_EQ(weapons.size(), 2U);
  EXPECT_EQ(weapons.at(0).name, std::string{RED_WEAPON});
  EXPECT_EQ(weapons.at(0).levels.size(), RED_LEVELS);
  EXPECT_EQ(weapons.at(1).name, std::string{BLUE_WEAPON});
  EXPECT_EQ(weapons.at(1).levels.size(), BLUE_LEVELS);
}

/**
 * Given the sheets of the armed ship and of its missiles
 * When the sheets are read
 * Then the first level of red fires fan_shot every 20 ticks, three at once
 * over 24 degrees, 16 units ahead
 */
TEST(EntitySheets, ReadsTheGunOfALevel) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, ARMED_SHIP_TEXT),
      sheetText(FAN_SHOT_NAME, FAN_SHOT_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  const GunSheet& level =
      sheets.named(ARMED_SHIP_NAME).weapons.at(0).levels.at(0);
  EXPECT_EQ(level.intervalTicks, FIRST_RED_INTERVAL_TICKS);
  EXPECT_EQ(level.projectileNumber, sheets.named(FAN_SHOT_NAME).number);
  EXPECT_EQ(level.pattern, FIRST_RED_PATTERN);
}

/**
 * Given the sheets of the armed ship, whose gun names no count, spread, step
 * or distance
 * When the sheets are read
 * Then its gun fires one projectile, straight ahead, at the muzzle
 */
TEST(EntitySheets, AGunFiresOneProjectileByDefault) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, ARMED_SHIP_TEXT),
      sheetText(FAN_SHOT_NAME, FAN_SHOT_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  const EntitySheet& ship = sheets.named(ARMED_SHIP_NAME);
  ASSERT_TRUE(ship.gun.has_value());
  EXPECT_EQ(ship.gun.value_or(GunSheet{}).pattern, ShotPattern{});
}

/**
 * Given the sheet of a Bydo that names a weapon, alone
 * When it is read
 * Then it is refused as inconsistent, before any projectile is looked for:
 * only a ship holds weapons
 */
TEST(EntitySheets, RefusesTheWeaponsOfABodyThatIsNoShip) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_BYDO_NAME, ARMED_BYDO_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}

/**
 * Given a ship with two weapons named red, and its missile
 * When the sheets are read
 * Then they are refused: a crystal names one weapon
 */
TEST(EntitySheets, RefusesTwoWeaponsOfOneName) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, TWINNED_NAME_SHIP_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}

/**
 * Given a ship whose weapon has three levels, and its missile
 * When the sheets are read
 * Then they are refused: a weapon has one or two levels
 */
TEST(EntitySheets, RefusesAWeaponOfThreeLevels) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, THREE_LEVEL_SHIP_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}

/**
 * Given a ship whose weapon spreads missiles that fly straight, and that
 * missile
 * When the sheets are read
 * Then they are refused: straight missiles would all fly along one row
 */
TEST(EntitySheets, RefusesAFanOfStraightMissiles) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, STRAIGHT_FAN_SHIP_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}

/**
 * Given a ship whose weapon lays missiles that scatter on its muzzle, and that
 * missile
 * When the sheets are read
 * Then they are refused: such a missile, on the point it flies away from,
 * would fly left
 */
TEST(EntitySheets, RefusesScatteringMissilesLaidOnTheMuzzle) {
  const std::vector<SheetText> texts{
      sheetText(ARMED_SHIP_NAME, SCATTER_ON_THE_MUZZLE_SHIP_TEXT),
      sheetText(FAN_SHOT_NAME, FAN_SHOT_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}
