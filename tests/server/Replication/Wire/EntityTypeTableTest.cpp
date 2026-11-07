#include <gtest/gtest.h>
#include <array>
#include <string>
#include "EntitySheets.hpp"
#include "EntityTypeTable.hpp"
#include "EntityTypeTableTestConstants.hpp"
#include "SheetText.hpp"
#include "ShipGun.hpp"
#include "ShippedEntitySheets.hpp"

using rtype::game::EntitySheets;
using rtype::game::SheetText;
using rtype::game::testing::shippedEntitySheets;
using rtype::network::ShipGun;
using rtype::server::shipGunOf;
using rtype::server::testing::SHIPPED_GUN_FIRST_SHOT_TICKS;
using rtype::server::testing::SHIPPED_GUN_INTERVAL_TICKS;
using rtype::server::testing::SHIPPED_PLAYER_MISSILE_NUMBER;
using rtype::server::testing::SHIPPED_PLAYER_MISSILE_SPEED;
using rtype::server::testing::SHIPPED_SHIP_NUMBER;
using rtype::server::testing::UNARMED_SHIP_SHEET_TEXT;

/**
 * Given the shipped sheets
 * When the gun of the ships is read from them
 * Then it is the one of ship, firing player_missile every 20 ticks from the
 * start, the missile flying right at 960 units per second
 */
TEST(ShipGunOf, IsTheGunOfTheShippedShip) {
  EXPECT_EQ(shipGunOf(shippedEntitySheets()),
            (ShipGun{.shipType = SHIPPED_SHIP_NUMBER,
                     .projectileType = SHIPPED_PLAYER_MISSILE_NUMBER,
                     .intervalTicks = SHIPPED_GUN_INTERVAL_TICKS,
                     .firstShotTicks = SHIPPED_GUN_FIRST_SHOT_TICKS,
                     .velocityX = SHIPPED_PLAYER_MISSILE_SPEED,
                     .velocityY = 0.0F}));
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
