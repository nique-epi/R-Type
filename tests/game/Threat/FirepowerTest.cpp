#include <gtest/gtest.h>
#include <cstdint>
#include "EntitySheet.hpp"
#include "EntityTypesTestConstants.hpp"
#include "Firepower.hpp"
#include "FirepowerFixtures.hpp"
#include "FirepowerTestConstants.hpp"
#include "Gun.hpp"
#include "Position.hpp"
#include "ShippedEntitySheets.hpp"
#include "TeamFirepower.hpp"
#include "World.hpp"

using rtype::game::EntitySheet;
using rtype::game::firepowerPercentOf;
using rtype::game::Gun;
using rtype::game::highestFirepowerPercentOf;
using rtype::game::Position;
using rtype::game::TeamFirepower;
using rtype::game::teamFirepowerOf;
using rtype::game::World;
using rtype::game::testing::AS_MUCH_AS_THE_SHEET;
using rtype::game::testing::FIRST_PLAYER;
using rtype::game::testing::FIRST_SHIP_X;
using rtype::game::testing::GUN_INTERVAL_TICKS;
using rtype::game::testing::gunFiring;
using rtype::game::testing::gunSheetFiring;
using rtype::game::testing::HALF_GUN_INTERVAL_TICKS;
using rtype::game::testing::MEAN_OF_THE_GUN_AND_THE_STRONGEST;
using rtype::game::testing::ONE_STRAIGHT_PROJECTILE;
using rtype::game::testing::RED_SECOND_LEVEL;
using rtype::game::testing::SECOND_PLAYER;
using rtype::game::testing::SECOND_SHIP_X;
using rtype::game::testing::SHIP_SHEET;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::SHIPS_Y;
using rtype::game::testing::SIDEWAYS_PAIR;
using rtype::game::testing::STRONGEST_SHIPPED_WEAPON;
using rtype::game::testing::THREE_STRAIGHT_PROJECTILES;
using rtype::game::testing::THREE_TIMES_THE_SHEET;
using rtype::game::testing::TWICE_THE_SHEET;
using rtype::game::testing::WIDE_FAN_OF_FIVE;
using rtype::game::testing::WIDE_FAN_OF_THREE;

/**
 * Given the gun of a sheet, one projectile straight ahead every 20 ticks
 * When a ship holds that same gun
 * Then it fires as much as its sheet: 100 %
 */
TEST(Firepower, TheGunOfTheSheetFiresAsMuchAsItself) {
  EXPECT_EQ(firepowerPercentOf(
                gunFiring(ONE_STRAIGHT_PROJECTILE, GUN_INTERVAL_TICKS),
                gunSheetFiring(ONE_STRAIGHT_PROJECTILE, GUN_INTERVAL_TICKS)),
            AS_MUCH_AS_THE_SHEET);
}

/**
 * Given the gun of a sheet, one projectile ahead every 20 ticks
 * When a ship fires three projectiles ahead every 20 ticks
 * Then it fires 300 % of its sheet
 */
TEST(Firepower, EachProjectileAheadCountsOnce) {
  EXPECT_EQ(firepowerPercentOf(
                gunFiring(THREE_STRAIGHT_PROJECTILES, GUN_INTERVAL_TICKS),
                gunSheetFiring(ONE_STRAIGHT_PROJECTILE, GUN_INTERVAL_TICKS)),
            THREE_TIMES_THE_SHEET);
}

/**
 * Given the gun of a sheet, one projectile ahead every 20 ticks
 * When a ship fires the same projectile every 10 ticks
 * Then it fires 200 % of its sheet
 */
TEST(Firepower, AGunThatFiresTwiceAsOftenFiresTwiceAsMuch) {
  EXPECT_EQ(firepowerPercentOf(
                gunFiring(ONE_STRAIGHT_PROJECTILE, HALF_GUN_INTERVAL_TICKS),
                gunSheetFiring(ONE_STRAIGHT_PROJECTILE, GUN_INTERVAL_TICKS)),
            TWICE_THE_SHEET);
}

/**
 * Given the gun of a sheet, one projectile ahead every 20 ticks
 * When a ship fires three projectiles over 160 degrees: ahead, 80 up, 80 down
 * Then only the one ahead counts: 100 %
 */
TEST(Firepower, AProjectileFiredWideOfTheConeDoesNotCount) {
  EXPECT_EQ(firepowerPercentOf(
                gunFiring(WIDE_FAN_OF_THREE, GUN_INTERVAL_TICKS),
                gunSheetFiring(ONE_STRAIGHT_PROJECTILE, GUN_INTERVAL_TICKS)),
            AS_MUCH_AS_THE_SHEET);
}

/**
 * Given the gun of a sheet, one projectile ahead every 20 ticks
 * When a ship fires five projectiles over 160 degrees, two of them at 40
 * Then the three within 45 degrees count: 300 %
 */
TEST(Firepower, AProjectileWithinTheConeCounts) {
  EXPECT_EQ(firepowerPercentOf(
                gunFiring(WIDE_FAN_OF_FIVE, GUN_INTERVAL_TICKS),
                gunSheetFiring(ONE_STRAIGHT_PROJECTILE, GUN_INTERVAL_TICKS)),
            THREE_TIMES_THE_SHEET);
}

/**
 * Given the gun of a sheet that fires only up and down, nothing ahead
 * When a ship fires three projectiles ahead
 * Then its firepower stays 100 %: there is nothing to compare it with
 */
TEST(Firepower, ASheetThatFiresNothingAheadGivesAsMuchAsItself) {
  EXPECT_EQ(firepowerPercentOf(
                gunFiring(THREE_STRAIGHT_PROJECTILES, GUN_INTERVAL_TICKS),
                gunSheetFiring(SIDEWAYS_PAIR, GUN_INTERVAL_TICKS)),
            AS_MUCH_AS_THE_SHEET);
}

/**
 * Given the shipped sheet of the ship and its red, blue and yellow weapons
 * When the strongest of its guns is asked for
 * Then it is 500 %: five projectiles ahead every 20 ticks for red, three
 * every 12 ticks for blue, against one every 20 ticks for the gun
 */
TEST(Firepower, TheStrongestShippedWeaponFiresFiveTimesTheGun) {
  EXPECT_EQ(highestFirepowerPercentOf(shippedEntitySheets().named(SHIP_SHEET)),
            STRONGEST_SHIPPED_WEAPON);
}

/**
 * Given a sheet with no gun
 * When the strongest of its guns is asked for
 * Then it is 100 %
 */
TEST(Firepower, ASheetWithoutAGunGivesAsMuchAsItself) {
  EXPECT_EQ(highestFirepowerPercentOf(EntitySheet{}), AS_MUCH_AS_THE_SHEET);
}

/**
 * Given a world with no ship
 * When the firepower of its team is asked for
 * Then the team fires as the gun of its sheet, with nothing stronger
 */
TEST(Firepower, ATeamWithoutAShipFiresAsItsSheet) {
  World world;

  EXPECT_EQ(teamFirepowerOf(world.components(), shippedEntitySheets()),
            TeamFirepower{});
}

/**
 * Given two shipped ships, one holding its gun and one the second level of
 * red
 * When the firepower of their team is asked for
 * Then it is the mean of 100 % and 500 %, and the strongest weapon is 500 %
 */
TEST(Firepower, ATeamFiresTheMeanOfItsShips) {
  World world;
  const EntitySheet& ship = shippedEntitySheets().named(SHIP_SHEET);
  world.spawn(ship, Position{.x = FIRST_SHIP_X, .y = SHIPS_Y}, FIRST_PLAYER);
  const std::uint32_t red = world.spawn(
      ship, Position{.x = SECOND_SHIP_X, .y = SHIPS_Y}, SECOND_PLAYER);
  Gun* const gun = world.shipComponentOf<Gun>(red);
  ASSERT_NE(gun, nullptr);
  *gun = gunFiring(RED_SECOND_LEVEL, GUN_INTERVAL_TICKS);

  EXPECT_EQ(teamFirepowerOf(world.components(), shippedEntitySheets()),
            (TeamFirepower{.meanPercent = MEAN_OF_THE_GUN_AND_THE_STRONGEST,
                           .highestPercent = STRONGEST_SHIPPED_WEAPON}));
}
