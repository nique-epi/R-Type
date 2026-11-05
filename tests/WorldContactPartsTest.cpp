#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include "CurrentAction.hpp"
#include "EntitySheet.hpp"
#include "EntityTypesTestConstants.hpp"
#include "HurtZone.hpp"
#include "Position.hpp"
#include "ShippedEntitySheets.hpp"
#include "SurvivesContact.hpp"
#include "World.hpp"
#include "WorldContactPartsFixtures.hpp"
#include "WorldFixtures.hpp"
#include "WorldTestConstants.hpp"

using rtype::game::CurrentAction;
using rtype::game::EntitySheet;
using rtype::game::HurtZone;
using rtype::game::Position;
using rtype::game::SurvivesContact;
using rtype::game::World;
using rtype::game::testing::componentOf;
using rtype::game::testing::DRIFTER_SHEET;
using rtype::game::testing::drifterWith;
using rtype::game::testing::SHIP_SHEET;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::SOME_X;
using rtype::game::testing::SOME_Y;
using rtype::game::testing::someHurtZone;

/**
 * Given the sheet of a Drifter, of side enemy
 * When an entity of that sheet is spawned
 * Then it has a CurrentAction that names no attack
 */
TEST(WorldContactParts, AnEnemyStartsIdle) {
  World world;

  const std::uint32_t entityId =
      world.spawn(shippedEntitySheets().named(DRIFTER_SHEET),
                  Position{.x = SOME_X, .y = SOME_Y});

  const std::optional<CurrentAction> action =
      componentOf<CurrentAction>(world, entityId);
  ASSERT_TRUE(action.has_value());
  EXPECT_FALSE(action.value_or(CurrentAction{}).key.has_value());
}

/**
 * Given the sheet of the ship of a player, of side player
 * When an entity of that sheet is spawned
 * Then it has no CurrentAction
 */
TEST(WorldContactParts, AShipHasNoCurrentAction) {
  World world;

  const std::uint32_t entityId =
      world.spawn(shippedEntitySheets().named(SHIP_SHEET),
                  Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_FALSE(componentOf<CurrentAction>(world, entityId).has_value());
}

/**
 * Given a sheet with a hurt zone of two boxes
 * When an entity of that sheet is spawned
 * Then it carries that hurt zone
 */
TEST(WorldContactParts, TheHurtZoneOfTheSheetIsOnTheEntity) {
  World world;

  const std::uint32_t entityId = world.spawn(
      drifterWith(someHurtZone(), false), Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_EQ(componentOf<HurtZone>(world, entityId), someHurtZone());
}

/**
 * Given a sheet whose enemy survives a contact
 * When an entity of that sheet is spawned
 * Then it carries the SurvivesContact marker
 */
TEST(WorldContactParts, TheMarkerOfTheSheetIsOnTheEntity) {
  World world;

  const std::uint32_t entityId = world.spawn(
      drifterWith(std::nullopt, true), Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_TRUE(componentOf<SurvivesContact>(world, entityId).has_value());
}

/**
 * Given the shipped sheet of a Drifter, which declares no contact part
 * When an entity of that sheet is spawned
 * Then it has neither a hurt zone nor the marker
 */
TEST(WorldContactParts, ASheetThatDeclaresNothingGivesNoContactPart) {
  World world;

  const std::uint32_t entityId =
      world.spawn(shippedEntitySheets().named(DRIFTER_SHEET),
                  Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_FALSE(componentOf<HurtZone>(world, entityId).has_value());
  EXPECT_FALSE(componentOf<SurvivesContact>(world, entityId).has_value());
}
