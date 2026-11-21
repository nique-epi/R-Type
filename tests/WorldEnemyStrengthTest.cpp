#include <gtest/gtest.h>
#include <cstdint>
#include "EntitySheet.hpp"
#include "EntityTypesTestConstants.hpp"
#include "Gun.hpp"
#include "GunSheet.hpp"
#include "Health.hpp"
#include "Position.hpp"
#include "ShippedEntitySheets.hpp"
#include "World.hpp"
#include "WorldEnemyStrengthTestConstants.hpp"
#include "WorldFixtures.hpp"
#include "WorldTestConstants.hpp"

using rtype::game::EntitySheet;
using rtype::game::Gun;
using rtype::game::GunSheet;
using rtype::game::Health;
using rtype::game::Position;
using rtype::game::World;
using rtype::game::testing::BYDO_TYRANT_SHEET;
using rtype::game::testing::componentOf;
using rtype::game::testing::DRIFTER_GUN_TICKS_HALVED;
using rtype::game::testing::DRIFTER_HEALTH_HALF_AGAIN;
using rtype::game::testing::DRIFTER_SHEET;
using rtype::game::testing::FASTEST_GUN_TICKS;
using rtype::game::testing::HALF_AGAIN_THE_HEALTH;
using rtype::game::testing::HALF_THE_GUN_INTERVAL;
using rtype::game::testing::ONE_PERCENT_OF_THE_GUN_INTERVAL;
using rtype::game::testing::SHIP_SHEET;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::SOME_X;
using rtype::game::testing::SOME_Y;
using rtype::game::testing::STRONGER_ON_EVERY_COUNT;

/**
 * Given a world whose enemies are made at 150 % of their health
 * When a Drifter, 3 points on its sheet, is spawned
 * Then it has 4 points, 4.5 rounded down, all of them
 */
TEST(WorldEnemyStrength, AnEnemyTakesTheHealthOfTheStrength) {
  World world;
  world.setEnemyStrength(HALF_AGAIN_THE_HEALTH);

  const std::uint32_t drifter =
      world.spawn(shippedEntitySheets().named(DRIFTER_SHEET),
                  Position{.x = SOME_X, .y = SOME_Y});

  const Health health = componentOf<Health>(world, drifter).value_or(Health{});
  EXPECT_EQ(health.points, DRIFTER_HEALTH_HALF_AGAIN);
  EXPECT_EQ(health.full, DRIFTER_HEALTH_HALF_AGAIN);
}

/**
 * Given a world whose enemies fire at 50 % of the ticks of their guns
 * When a Drifter, 90 ticks between shots and before the first, is spawned
 * Then it fires every 45 ticks, the first after 45
 */
TEST(WorldEnemyStrength, AnEnemyTakesTheGunOfTheStrength) {
  World world;
  world.setEnemyStrength(HALF_THE_GUN_INTERVAL);

  const std::uint32_t drifter =
      world.spawn(shippedEntitySheets().named(DRIFTER_SHEET),
                  Position{.x = SOME_X, .y = SOME_Y});

  const Gun gun = componentOf<Gun>(world, drifter).value_or(Gun{});
  EXPECT_EQ(gun.intervalTicks, DRIFTER_GUN_TICKS_HALVED);
  EXPECT_EQ(gun.ticksUntilReady, DRIFTER_GUN_TICKS_HALVED);
}

/**
 * Given a world whose enemies fire at 1 % of the ticks of their guns
 * When a Drifter, 90 ticks between shots, is spawned
 * Then it fires every tick at most, never more often
 */
TEST(WorldEnemyStrength, AGunNeverFiresMoreThanOnceATick) {
  World world;
  world.setEnemyStrength(ONE_PERCENT_OF_THE_GUN_INTERVAL);

  const std::uint32_t drifter =
      world.spawn(shippedEntitySheets().named(DRIFTER_SHEET),
                  Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_EQ(componentOf<Gun>(world, drifter).value_or(Gun{}).intervalTicks,
            FASTEST_GUN_TICKS);
}

/**
 * Given a world whose enemies are made at 200 % of their health
 * When the Bydo Tyrant, a boss of 240 points, is spawned
 * Then it keeps the points of its sheet: BossSystem scales a boss
 */
TEST(WorldEnemyStrength, ABossKeepsTheHealthOfItsSheet) {
  World world;
  world.setEnemyStrength(STRONGER_ON_EVERY_COUNT);
  const EntitySheet& sheet = shippedEntitySheets().named(BYDO_TYRANT_SHEET);

  const std::uint32_t tyrant =
      world.spawn(sheet, Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_EQ(componentOf<Health>(world, tyrant).value_or(Health{}).points,
            sheet.health.value_or(0));
}

/**
 * Given a world whose enemies fire at 50 % of the ticks of their guns
 * When the ship of a player is spawned
 * Then it keeps the ticks of the gun of its sheet
 */
TEST(WorldEnemyStrength, TheShipKeepsTheGunOfItsSheet) {
  World world;
  world.setEnemyStrength(STRONGER_ON_EVERY_COUNT);
  const EntitySheet& sheet = shippedEntitySheets().named(SHIP_SHEET);

  const std::uint32_t ship =
      world.spawn(sheet, Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_EQ(componentOf<Gun>(world, ship).value_or(Gun{}).intervalTicks,
            sheet.gun.value_or(GunSheet{}).intervalTicks);
}

/**
 * Given a Drifter spawned before the world made its enemies stronger
 * When the strength is set to 200 % of the health
 * Then the Drifter keeps the points of its sheet it appeared with
 */
TEST(WorldEnemyStrength, AnEnemyAlreadyInTheWorldKeepsItsHealth) {
  World world;
  const EntitySheet& sheet = shippedEntitySheets().named(DRIFTER_SHEET);
  const std::uint32_t drifter =
      world.spawn(sheet, Position{.x = SOME_X, .y = SOME_Y});

  world.setEnemyStrength(STRONGER_ON_EVERY_COUNT);

  EXPECT_EQ(componentOf<Health>(world, drifter).value_or(Health{}).points,
            sheet.health.value_or(0));
}
