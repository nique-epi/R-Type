#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include "EntitySheet.hpp"
#include "EntityTypesTestConstants.hpp"
#include "Gun.hpp"
#include "GunSheet.hpp"
#include "ShippedEntitySheets.hpp"
#include "WeaponPickup.hpp"
#include "WeaponPickupTestConstants.hpp"

using rtype::game::EntitySheet;
using rtype::game::Gun;
using rtype::game::GunSheet;
using rtype::game::testing::SHIP_SHEET;
using rtype::game::testing::shippedEntitySheets;

namespace rtype::server::testing {

/**
 * Given the shipped ship holding the gun of its sheet
 * When it picks up a red crystal
 * Then it fires the first level of red: its interval, its shot and its fan
 */
TEST(WeaponPickup, AFirstCrystalGivesTheFirstLevel) {
  const EntitySheet& ship = shippedEntitySheets().named(SHIP_SHEET);
  const GunSheet& red = ship.weapons.at(RED_INDEX).levels.at(0);

  const std::optional<Gun> gun = gunAfterPickup(Gun{}, ship, RED_WEAPON_NAME);

  ASSERT_TRUE(gun.has_value());
  const Gun armed = gun.value_or(Gun{});
  EXPECT_EQ(armed.weapon, RED_WEAPON_NUMBER);
  EXPECT_EQ(armed.level, FIRST_LEVEL);
  EXPECT_EQ(armed.intervalTicks, red.intervalTicks);
  EXPECT_EQ(armed.projectileNumber, red.projectileNumber);
  EXPECT_EQ(armed.pattern, red.pattern);
}

/**
 * Given the shipped ship holding the first level of red
 * When it picks up a second red crystal
 * Then it holds the second level of red
 */
TEST(WeaponPickup, ACrystalOfTheWeaponHeldRaisesItsLevel) {
  const EntitySheet& ship = shippedEntitySheets().named(SHIP_SHEET);
  const Gun held{.weapon = RED_WEAPON_NUMBER, .level = FIRST_LEVEL};

  const Gun armed = gunAfterPickup(held, ship, RED_WEAPON_NAME).value_or(Gun{});

  EXPECT_EQ(armed.weapon, RED_WEAPON_NUMBER);
  EXPECT_EQ(armed.level, SECOND_LEVEL);
  EXPECT_EQ(armed.pattern, ship.weapons.at(RED_INDEX).levels.at(1).pattern);
}

/**
 * Given the shipped ship holding the second level of red, its last
 * When it picks up a third red crystal
 * Then it keeps the second level
 */
TEST(WeaponPickup, ALevelNeverGoesPastTheLast) {
  const EntitySheet& ship = shippedEntitySheets().named(SHIP_SHEET);
  const Gun held{.weapon = RED_WEAPON_NUMBER, .level = SECOND_LEVEL};

  const Gun armed = gunAfterPickup(held, ship, RED_WEAPON_NAME).value_or(Gun{});

  EXPECT_EQ(armed.level, SECOND_LEVEL);
}

/**
 * Given the shipped ship holding the second level of red
 * When it picks up a blue crystal
 * Then it holds the first level of blue
 */
TEST(WeaponPickup, ACrystalOfAnotherWeaponStartsAtItsFirstLevel) {
  const EntitySheet& ship = shippedEntitySheets().named(SHIP_SHEET);
  const Gun held{.weapon = RED_WEAPON_NUMBER, .level = SECOND_LEVEL};

  const Gun armed =
      gunAfterPickup(held, ship, BLUE_WEAPON_NAME).value_or(Gun{});

  EXPECT_EQ(armed.weapon, BLUE_WEAPON_NUMBER);
  EXPECT_EQ(armed.level, FIRST_LEVEL);
}

/**
 * Given the shipped ship, which holds no weapon named green
 * When it picks up a crystal of green
 * Then it gets no gun: the crystal is left
 */
TEST(WeaponPickup, ACrystalOfAWeaponTheShipLacksGivesNothing) {
  const EntitySheet& ship = shippedEntitySheets().named(SHIP_SHEET);

  EXPECT_FALSE(gunAfterPickup(Gun{}, ship, GREEN_WEAPON_NAME).has_value());
}

/**
 * Given a ship whose gun waits 600 ticks before its next shot, then one that
 * waits 3
 * When each picks up a red crystal
 * Then the first waits no longer than an interval of red, the second keeps
 * its 3 ticks
 */
TEST(WeaponPickup, AWeaponNeverWaitsLongerThanItsInterval) {
  const EntitySheet& ship = shippedEntitySheets().named(SHIP_SHEET);
  const std::size_t interval =
      ship.weapons.at(RED_INDEX).levels.at(0).intervalTicks;

  const Gun fromLong = gunAfterPickup(Gun{.ticksUntilReady = LONG_WAIT_TICKS},
                                      ship, RED_WEAPON_NAME)
                           .value_or(Gun{});
  const Gun fromShort = gunAfterPickup(Gun{.ticksUntilReady = SHORT_WAIT_TICKS},
                                       ship, RED_WEAPON_NAME)
                            .value_or(Gun{});

  EXPECT_EQ(fromLong.ticksUntilReady, interval);
  EXPECT_EQ(fromShort.ticksUntilReady, SHORT_WAIT_TICKS);
}

}  // namespace rtype::server::testing
