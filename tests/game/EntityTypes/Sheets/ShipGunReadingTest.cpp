#include <gtest/gtest.h>
#include <array>
#include <string>
#include <vector>
#include "EntitySheets.hpp"
#include "EntityTypesTestConstants.hpp"
#include "HeldWeapon.hpp"
#include "SheetText.hpp"
#include "SheetTextReading.hpp"
#include "ShipArsenal.hpp"
#include "ShipGun.hpp"
#include "ShipGunReading.hpp"
#include "ShipGunReadingFixtures.hpp"
#include "ShipGunReadingTestConstants.hpp"
#include "ShippedEntitySheets.hpp"

using rtype::game::EntitySheets;
using rtype::game::heldGunOf;
using rtype::game::SheetText;
using rtype::game::sheetTextsIn;
using rtype::game::ShipArsenal;
using rtype::game::shipArsenalOf;
using rtype::game::shipArsenalOrNoneOf;
using rtype::game::ShipGun;
using rtype::game::testing::A_LEVEL_PAST_THE_LAST;
using rtype::game::testing::A_WEAPON_PAST_THE_LAST;
using rtype::game::testing::BLUE_FIRST_LEVEL_COUNT;
using rtype::game::testing::BLUE_FIRST_LEVEL_INTERVAL_TICKS;
using rtype::game::testing::BLUE_PLACE;
using rtype::game::testing::BLUE_SHOT_SHEET;
using rtype::game::testing::BLUE_SHOT_SPEED;
using rtype::game::testing::FIRST_LEVEL_PLACE;
using rtype::game::testing::PLUGIN_PILOT_SHEET_TEXT;
using rtype::game::testing::RED_AT_ITS_SECOND_LEVEL;
using rtype::game::testing::RED_PLACE;
using rtype::game::testing::RED_SECOND_LEVEL_COUNT;
using rtype::game::testing::RED_SECOND_LEVEL_SPREAD_DEGREES;
using rtype::game::testing::RED_SHOT_SHEET;
using rtype::game::testing::RED_SHOT_SPEED;
using rtype::game::testing::SECOND_LEVEL_PLACE;
using rtype::game::testing::SHIPPED_LEVELS_PER_WEAPON;
using rtype::game::testing::SHIPPED_WEAPON_COUNT;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::shippedGun;
using rtype::game::testing::shippedSheetFolder;
using rtype::game::testing::UNARMED_SHIP_SHEET_TEXT;

/**
 * Given the shipped sheets
 * When the gun of the ships is read from them
 * Then it is the one of ship, firing player_missile every 20 ticks from the
 * start, the missile flying right at 960 units per second
 */
TEST(ShipGunOf, IsTheGunOfTheShippedShip) {
  EXPECT_EQ(shipArsenalOf(shippedEntitySheets()).gun, shippedGun());
}

/**
 * Given sheets whose only ship fires nothing
 * When the gun of the ships is read from them
 * Then it names no type and is 0 everywhere
 */
TEST(ShipGunOf, IsNothingForAShipWithoutGun) {
  const std::array texts{
      SheetText{.name = "ship",
                .source = "ship.toml",
                .text = std::string{UNARMED_SHIP_SHEET_TEXT}}};

  EXPECT_EQ(shipArsenalOf(EntitySheets::fromTexts(texts)), ShipArsenal{});
}

/**
 * Given the texts of the shipped sheets, as a client reads them
 * When the gun of the ships is read from them
 * Then it is the gun of the shipped ship
 */
TEST(ShipGunOrNoneOf, IsTheGunOfTheShippedShipFromItsTexts) {
  const std::vector<SheetText> texts = sheetTextsIn(shippedSheetFolder());

  EXPECT_EQ(shipArsenalOrNoneOf(texts).gun, shippedGun());
}

/**
 * Given the texts of a server whose Bydo is piloted by a behaviour only a
 * plugin knows, next to the shipped ship
 * When a client reads the gun of the ships from them
 * Then it predicts nothing, rather than refusing the content
 */
TEST(ShipGunOrNoneOf, PredictsNothingWhenABehaviourIsUnknown) {
  std::vector<SheetText> texts = sheetTextsIn(shippedSheetFolder());
  texts.push_back(SheetText{.name = "lancer",
                            .source = "lancer.toml",
                            .text = std::string{PLUGIN_PILOT_SHEET_TEXT}});

  EXPECT_EQ(shipArsenalOrNoneOf(texts), ShipArsenal{});
}

/**
 * Given the shipped sheets, whose ship holds red, blue and yellow
 * When the guns of the ships are read from them
 * Then there are three weapons of two levels each
 */
TEST(ShipArsenalOf, ReadsEachLevelOfEachWeapon) {
  const ShipArsenal arsenal = shipArsenalOf(shippedEntitySheets());

  ASSERT_EQ(arsenal.weapons.size(), SHIPPED_WEAPON_COUNT);
  for (const std::vector<ShipGun>& levels : arsenal.weapons) {
    EXPECT_EQ(levels.size(), SHIPPED_LEVELS_PER_WEAPON);
  }
}

/**
 * Given the shipped sheets
 * When the second level of red is read
 * Then it fires five red shots over 48 degrees, which scatter at 960 units
 * per second
 */
TEST(ShipArsenalOf, ReadsAFanThatScatters) {
  const ShipArsenal arsenal = shipArsenalOf(shippedEntitySheets());
  ASSERT_EQ(arsenal.weapons.size(), SHIPPED_WEAPON_COUNT);

  const ShipGun& red = arsenal.weapons.at(RED_PLACE).at(SECOND_LEVEL_PLACE);

  EXPECT_EQ(red.projectileType,
            shippedEntitySheets().named(RED_SHOT_SHEET).number);
  EXPECT_EQ(red.pattern.count, RED_SECOND_LEVEL_COUNT);
  EXPECT_EQ(red.pattern.spreadDegrees, RED_SECOND_LEVEL_SPREAD_DEGREES);
  EXPECT_EQ(red.scatterSpeed, RED_SHOT_SPEED);
  EXPECT_EQ(red.velocityX, 0.0F);
}

/**
 * Given the shipped sheets
 * When the first level of blue is read
 * Then it fires two blue shots every 15 ticks, flying right at 1440 units per
 * second, which do not scatter
 */
TEST(ShipArsenalOf, ReadsShotsThatFlyStraight) {
  const ShipArsenal arsenal = shipArsenalOf(shippedEntitySheets());
  ASSERT_EQ(arsenal.weapons.size(), SHIPPED_WEAPON_COUNT);

  const ShipGun& blue = arsenal.weapons.at(BLUE_PLACE).at(FIRST_LEVEL_PLACE);

  EXPECT_EQ(blue.projectileType,
            shippedEntitySheets().named(BLUE_SHOT_SHEET).number);
  EXPECT_EQ(blue.pattern.count, BLUE_FIRST_LEVEL_COUNT);
  EXPECT_EQ(blue.intervalTicks, BLUE_FIRST_LEVEL_INTERVAL_TICKS);
  EXPECT_EQ(blue.velocityX, BLUE_SHOT_SPEED);
  EXPECT_EQ(blue.scatterSpeed, 0.0F);
}

/**
 * Given the guns of the shipped ship
 * When a ship holds red at its second level
 * Then it fires the second level of red
 */
TEST(HeldGunOf, IsTheLevelOfTheWeaponHeld) {
  const ShipArsenal arsenal = shipArsenalOf(shippedEntitySheets());
  ASSERT_EQ(arsenal.weapons.size(), SHIPPED_WEAPON_COUNT);

  EXPECT_EQ(heldGunOf(arsenal, RED_AT_ITS_SECOND_LEVEL),
            arsenal.weapons.at(RED_PLACE).at(SECOND_LEVEL_PLACE));
}

/**
 * Given the guns of the shipped ship
 * When a ship holds its gun, then a weapon past the last, then a level past
 * the last of red
 * Then each fires the gun of the sheet
 */
TEST(HeldGunOf, IsTheGunOfTheSheetForWhatTheShipDoesNotHold) {
  const ShipArsenal arsenal = shipArsenalOf(shippedEntitySheets());

  EXPECT_EQ(heldGunOf(arsenal, {}), arsenal.gun);
  EXPECT_EQ(heldGunOf(arsenal, A_WEAPON_PAST_THE_LAST), arsenal.gun);
  EXPECT_EQ(heldGunOf(arsenal, A_LEVEL_PAST_THE_LAST), arsenal.gun);
}
