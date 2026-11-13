#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "CurrentAction.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityType.hpp"
#include "EntityTypesTestConstants.hpp"
#include "Fragile.hpp"
#include "HitboxParts.hpp"
#include "HurtZone.hpp"
#include "Position.hpp"
#include "ShippedEntitySheets.hpp"
#include "Shootable.hpp"
#include "SurvivesContact.hpp"
#include "World.hpp"
#include "WorldContactPartsFixtures.hpp"
#include "WorldContactPartsTestConstants.hpp"
#include "WorldFixtures.hpp"
#include "WorldTestConstants.hpp"

using rtype::game::CurrentAction;
using rtype::game::EntitySheet;
using rtype::game::EntityType;
using rtype::game::Fragile;
using rtype::game::HitboxParts;
using rtype::game::HurtZone;
using rtype::game::Position;
using rtype::game::Shootable;
using rtype::game::SHOOTABLE_MISSILE_SHEETS;
using rtype::game::SurvivesContact;
using rtype::game::World;
using rtype::game::testing::componentOf;
using rtype::game::testing::DRIFTER_SHEET;
using rtype::game::testing::drifterWith;
using rtype::game::testing::ENEMY_MISSILE_SHEET;
using rtype::game::testing::SHIP_SHEET;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::SOME_X;
using rtype::game::testing::SOME_Y;
using rtype::game::testing::someHitboxParts;
using rtype::game::testing::someHurtZone;
using rtype::game::testing::STUNNED_IN_ATTACK_ZERO;
using rtype::game::testing::UNLISTED_MISSILE_SHEET;

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

/**
 * Given a sheet whose enemy is fragile in the recover of its attack 0
 * When an entity of that sheet is spawned
 * Then it carries a Fragile naming that recover
 */
TEST(WorldContactParts, TheFragileActionOfTheSheetIsOnTheEntity) {
  World world;
  EntitySheet sheet = drifterWith(std::nullopt, false);
  sheet.fragileIn = STUNNED_IN_ATTACK_ZERO;

  const std::uint32_t entityId =
      world.spawn(sheet, Position{.x = SOME_X, .y = SOME_Y});

  const std::optional<Fragile> fragile = componentOf<Fragile>(world, entityId);
  ASSERT_TRUE(fragile.has_value());
  EXPECT_EQ(fragile.value_or(Fragile{}).in, STUNNED_IN_ATTACK_ZERO);
}

/**
 * Given a sheet that lists two boxes
 * When an entity of that sheet is spawned
 * Then it carries those boxes as its HitboxParts
 */
TEST(WorldContactParts, TheBoxesOfTheSheetAreOnTheEntity) {
  World world;
  EntitySheet sheet = drifterWith(std::nullopt, false);
  sheet.hitboxParts = someHitboxParts().boxes;

  const std::uint32_t entityId =
      world.spawn(sheet, Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_EQ(componentOf<HitboxParts>(world, entityId), someHitboxParts());
}

/**
 * Given the sheet of a Drifter, of one box
 * When an entity of that sheet is spawned
 * Then it has no HitboxParts
 */
TEST(WorldContactParts, ABodyOfOneBoxHasNoHitboxParts) {
  World world;

  const std::uint32_t entityId =
      world.spawn(shippedEntitySheets().named(DRIFTER_SHEET),
                  Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_FALSE(componentOf<HitboxParts>(world, entityId).has_value());
}

/**
 * Given the shipped sheet of the missile of a Bydo, which
 * SHOOTABLE_MISSILE_SHEETS names
 * When an entity of that sheet is spawned
 * Then it carries the Shootable marker
 */
TEST(WorldContactParts, AMissileOfTheListIsShootable) {
  World world;

  const std::uint32_t entityId =
      world.spawn(shippedEntitySheets().named(ENEMY_MISSILE_SHEET),
                  Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_TRUE(componentOf<Shootable>(world, entityId).has_value());
}

/**
 * Given the same sheet under a name SHOOTABLE_MISSILE_SHEETS does not hold
 * When an entity of that sheet is spawned
 * Then it carries no Shootable marker
 */
TEST(WorldContactParts, AMissileLeftOutOfTheListIsNotShootable) {
  World world;
  EntitySheet unlisted = shippedEntitySheets().named(ENEMY_MISSILE_SHEET);
  unlisted.name = std::string{UNLISTED_MISSILE_SHEET};

  const std::uint32_t entityId =
      world.spawn(unlisted, Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_FALSE(componentOf<Shootable>(world, entityId).has_value());
}

/**
 * Given the sheet of a Drifter, of side enemy, under a name
 * SHOOTABLE_MISSILE_SHEETS holds
 * When an entity of that sheet is spawned
 * Then it carries no Shootable marker: only a missile of an enemy is shot
 * down
 */
TEST(WorldContactParts, AnEnemyNamedInTheListIsNotShootable) {
  World world;
  EntitySheet enemy = shippedEntitySheets().named(DRIFTER_SHEET);
  enemy.name = std::string{ENEMY_MISSILE_SHEET};

  const std::uint32_t entityId =
      world.spawn(enemy, Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_FALSE(componentOf<Shootable>(world, entityId).has_value());
}

/**
 * Given the names SHOOTABLE_MISSILE_SHEETS holds
 * When each is looked up among the shipped sheets
 * Then each names a sheet of side enemy_missile, so no name of the list is
 * misspelt
 */
TEST(WorldContactParts, EveryShootableNameIsAShippedMissileOfAnEnemy) {
  for (const std::string_view name : SHOOTABLE_MISSILE_SHEETS) {
    EXPECT_EQ(shippedEntitySheets().named(name).side, EntityType::EnemyMissile)
        << name;
  }
}
