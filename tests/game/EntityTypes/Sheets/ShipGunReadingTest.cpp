#include <gtest/gtest.h>
#include <array>
#include <string>
#include <vector>
#include "EntitySheets.hpp"
#include "SheetText.hpp"
#include "SheetTextReading.hpp"
#include "ShipGun.hpp"
#include "ShipGunReading.hpp"
#include "ShipGunReadingTestConstants.hpp"
#include "ShippedEntitySheets.hpp"

using rtype::game::EntitySheets;
using rtype::game::SheetText;
using rtype::game::sheetTextsIn;
using rtype::game::ShipGun;
using rtype::game::shipGunOf;
using rtype::game::shipGunOrNoneOf;
using rtype::game::testing::PLUGIN_PILOT_SHEET_TEXT;
using rtype::game::testing::SHIPPED_GUN_FIRST_SHOT_TICKS;
using rtype::game::testing::SHIPPED_GUN_INTERVAL_TICKS;
using rtype::game::testing::SHIPPED_PLAYER_MISSILE_NUMBER;
using rtype::game::testing::SHIPPED_PLAYER_MISSILE_SPEED;
using rtype::game::testing::SHIPPED_SHIP_NUMBER;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::shippedSheetFolder;
using rtype::game::testing::UNARMED_SHIP_SHEET_TEXT;

namespace {

/** @returns The gun of the shipped ship, as the shipped sheets describe it. */
ShipGun shippedGun() {
  return ShipGun{.shipType = SHIPPED_SHIP_NUMBER,
                 .projectileType = SHIPPED_PLAYER_MISSILE_NUMBER,
                 .intervalTicks = SHIPPED_GUN_INTERVAL_TICKS,
                 .firstShotTicks = SHIPPED_GUN_FIRST_SHOT_TICKS,
                 .velocityX = SHIPPED_PLAYER_MISSILE_SPEED,
                 .velocityY = 0.0F};
}

}  // namespace

/**
 * Given the shipped sheets
 * When the gun of the ships is read from them
 * Then it is the one of ship, firing player_missile every 20 ticks from the
 * start, the missile flying right at 960 units per second
 */
TEST(ShipGunOf, IsTheGunOfTheShippedShip) {
  EXPECT_EQ(shipGunOf(shippedEntitySheets()), shippedGun());
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

  EXPECT_EQ(shipGunOf(EntitySheets::fromTexts(texts)), ShipGun{});
}

/**
 * Given the texts of the shipped sheets, as a client reads them
 * When the gun of the ships is read from them
 * Then it is the gun of the shipped ship
 */
TEST(ShipGunOrNoneOf, IsTheGunOfTheShippedShipFromItsTexts) {
  const std::vector<SheetText> texts = sheetTextsIn(shippedSheetFolder());

  EXPECT_EQ(shipGunOrNoneOf(texts), shippedGun());
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

  EXPECT_EQ(shipGunOrNoneOf(texts), ShipGun{});
}
