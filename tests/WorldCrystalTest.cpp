#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <vector>
#include "CarriedCrystals.hpp"
#include "EntityTypesTestConstants.hpp"
#include "Position.hpp"
#include "ShippedEntitySheets.hpp"
#include "World.hpp"
#include "WorldFixtures.hpp"
#include "WorldTestConstants.hpp"

using rtype::game::CarriedCrystals;
using rtype::game::Position;
using rtype::game::World;
using rtype::game::testing::componentOf;
using rtype::game::testing::CRYSTAL_BLUE_SHEET;
using rtype::game::testing::CRYSTAL_CARRIER_SHEET;
using rtype::game::testing::CRYSTAL_RED_SHEET;
using rtype::game::testing::CRYSTAL_YELLOW_SHEET;
using rtype::game::testing::DRIFTER_SHEET;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::SOME_X;
using rtype::game::testing::SOME_Y;

/**
 * Given the shipped sheet of the crystal carrier, which drops red, blue and
 * yellow
 * When a carrier is spawned
 * Then it carries the three crystals, in the order of its sheet
 */
TEST(WorldCrystal, ACarrierCarriesTheCrystalsOfItsSheet) {
  World world;

  const std::uint32_t carrier =
      world.spawn(shippedEntitySheets().named(CRYSTAL_CARRIER_SHEET),
                  Position{.x = SOME_X, .y = SOME_Y});

  const std::optional<CarriedCrystals> carried =
      componentOf<CarriedCrystals>(world, carrier);
  ASSERT_TRUE(carried.has_value());
  const std::vector<std::uint8_t> expected{
      shippedEntitySheets().named(CRYSTAL_RED_SHEET).number,
      shippedEntitySheets().named(CRYSTAL_BLUE_SHEET).number,
      shippedEntitySheets().named(CRYSTAL_YELLOW_SHEET).number};
  EXPECT_EQ(carried.value_or(CarriedCrystals{}).sheetNumbers, expected);
}

/**
 * Given the shipped sheet of the Drifter, which drops nothing
 * When a Drifter is spawned
 * Then it carries no crystal
 */
TEST(WorldCrystal, AnEnemyThatDropsNothingCarriesNoCrystal) {
  World world;

  const std::uint32_t drifter =
      world.spawn(shippedEntitySheets().named(DRIFTER_SHEET),
                  Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_FALSE(componentOf<CarriedCrystals>(world, drifter).has_value());
}
