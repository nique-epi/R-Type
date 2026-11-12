#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "Collision.hpp"
#include "CollisionFilter.hpp"
#include "CollisionRules.hpp"
#include "CollisionRulesFixtures.hpp"
#include "CollisionScene.hpp"
#include "CollisionSystem.hpp"
#include "CollisionTestConstants.hpp"
#include "Entity.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntityType.hpp"
#include "LookItemTouched.hpp"
#include "MissileHit.hpp"
#include "ShipContact.hpp"
#include "ShippedEntitySheets.hpp"
#include "SystemScheduler.hpp"
#include "TimeConstants.hpp"
#include "ViewDelay.hpp"

using rtype::engine::Collision;
using rtype::engine::CollisionFilter;
using rtype::engine::Entity;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::engine::SystemScheduler;
using rtype::game::CollisionRules;
using rtype::game::CollisionSystem;
using rtype::game::EntitySheet;
using rtype::game::EntityType;
using rtype::game::LookItemTouched;
using rtype::game::MissileHit;
using rtype::game::ShipContact;
using rtype::game::ViewDelay;
using rtype::game::testing::CollisionRulesInAnyOrder;
using rtype::game::testing::expectNoEventFor;
using rtype::game::testing::record;
using rtype::game::testing::Recorded;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::spawnMissileOf;
using rtype::game::testing::spawnShipFlownBy;

/**
 * Given a player missile and an enemy
 * When a Collision names them, in either order
 * Then one MissileHit is published, with the missile and the enemy as target
 */
TEST_P(CollisionRulesInAnyOrder, PlayerMissileHitsEnemy) {
  CollisionScene scene;
  const Entity missile = scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(missile, enemy));
  scene.events.dispatch();

  EXPECT_EQ(recorded.missileHits, (std::vector<MissileHit>{MissileHit{
                                      .missile = missile, .target = enemy}}));
  EXPECT_TRUE(recorded.shipContacts.empty());
}

/**
 * Given an enemy missile and a player
 * When a Collision names them, in either order
 * Then one MissileHit is published, with the missile and the player as target
 */
TEST_P(CollisionRulesInAnyOrder, EnemyMissileHitsPlayer) {
  CollisionScene scene;
  const Entity missile = scene.spawn(EntityType::EnemyMissile, 0.0F, 0.0F);
  const Entity player = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(missile, player));
  scene.events.dispatch();

  EXPECT_EQ(recorded.missileHits, (std::vector<MissileHit>{MissileHit{
                                      .missile = missile, .target = player}}));
  EXPECT_TRUE(recorded.shipContacts.empty());
}

/**
 * Given a player and an enemy
 * When a Collision names them, in either order
 * Then one ShipContact is published, naming the player and the enemy
 */
TEST_P(CollisionRulesInAnyOrder, PlayerTouchesEnemy) {
  CollisionScene scene;
  const Entity player = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(player, enemy));
  scene.events.dispatch();

  EXPECT_EQ(recorded.shipContacts, (std::vector<ShipContact>{ShipContact{
                                       .player = player, .enemy = enemy}}));
  EXPECT_TRUE(recorded.missileHits.empty());
}

/**
 * Given friendly fire on, a missile of one player and the ship of another
 * When a Collision names them, in either order
 * Then one MissileHit is published, with the missile and the ship as target
 */
TEST_P(CollisionRulesInAnyOrder, MissileHitsAnotherPilotWithFriendlyFire) {
  CollisionScene scene;
  const Entity missile = spawnMissileOf(scene, FIRING_PLAYER_NUMBER);
  const Entity ship = spawnShipFlownBy(scene, OTHER_PLAYER_NUMBER);
  const CollisionRules rules(scene.events, scene.components, true);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(missile, ship));
  scene.events.dispatch();

  EXPECT_EQ(recorded.missileHits, (std::vector<MissileHit>{MissileHit{
                                      .missile = missile, .target = ship}}));
}

/**
 * Given a player missile that carries a ViewDelay, and an enemy
 * When a Collision names them, in either order
 * Then no MissileHit is published: LagCompensatedHitSystem tests that missile
 * where its player saw the enemies
 */
TEST_P(CollisionRulesInAnyOrder, LeavesACompensatedMissileToTheLagSystem) {
  CollisionScene scene;
  const Entity missile = scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  scene.components.add(missile, ViewDelay{.ticks = 0});
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(missile, enemy));
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
}

/**
 * Given an enemy missile and a ship that carries a ViewDelay
 * When a Collision names them, in either order
 * Then no MissileHit is published: LagCompensatedHitSystem tests that ship
 * where its player saw the enemies
 */
TEST_P(CollisionRulesInAnyOrder, LeavesACompensatedShipHitToTheLagSystem) {
  CollisionScene scene;
  const Entity missile = scene.spawn(EntityType::EnemyMissile, 0.0F, 0.0F);
  const Entity player = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  scene.components.add(player, ViewDelay{.ticks = 0});
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(missile, player));
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
}

/**
 * Given a ship that carries a ViewDelay and an enemy
 * When a Collision names them, in either order
 * Then no ShipContact is published: LagCompensatedHitSystem tests that ship
 * where its player saw the enemies
 */
TEST_P(CollisionRulesInAnyOrder, LeavesACompensatedShipContactToTheLagSystem) {
  CollisionScene scene;
  const Entity player = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  scene.components.add(player, ViewDelay{.ticks = 0});
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(player, enemy));
  scene.events.dispatch();

  EXPECT_TRUE(recorded.shipContacts.empty());
}

/**
 * Given a player and a look item
 * When a Collision names them, in either order
 * Then one LookItemTouched is published, naming the ship and the item
 */
TEST_P(CollisionRulesInAnyOrder, AShipTouchingALookItemPublishesOneTouch) {
  CollisionScene scene;
  const Entity ship = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  const Entity item = scene.spawn(EntityType::LookItem, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(ship, item));
  scene.events.dispatch();

  EXPECT_EQ(recorded.lookItemTouches,
            (std::vector<LookItemTouched>{
                LookItemTouched{.ship = ship, .item = item}}));
  EXPECT_TRUE(recorded.missileHits.empty());
  EXPECT_TRUE(recorded.shipContacts.empty());
}

INSTANTIATE_TEST_SUITE_P(BothOrders, CollisionRulesInAnyOrder,
                         ::testing::Bool());

/**
 * Given friendly fire on, a missile of a player and the ship that player flies
 * When a Collision names them
 * Then nothing is published: a pilot never hits itself
 */
TEST(CollisionRules, MissileNeverHitsItsOwnPilot) {
  CollisionScene scene;
  const Entity missile = spawnMissileOf(scene, FIRING_PLAYER_NUMBER);
  const Entity ship = spawnShipFlownBy(scene, FIRING_PLAYER_NUMBER);
  const CollisionRules rules(scene.events, scene.components, true);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(Collision{.first = missile, .second = ship});
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
}

/**
 * Given friendly fire off, a missile of one player and the ship of another
 * When a Collision names them
 * Then nothing is published: the missile crosses the ship
 */
TEST(CollisionRules, MissileCrossesAnotherPilotWithoutFriendlyFire) {
  CollisionScene scene;
  const Entity missile = spawnMissileOf(scene, FIRING_PLAYER_NUMBER);
  const Entity ship = spawnShipFlownBy(scene, OTHER_PLAYER_NUMBER);
  const CollisionRules rules(scene.events, scene.components, false);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(Collision{.first = missile, .second = ship});
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
}

/**
 * Given friendly fire on, a player missile of no player and a ship of a pilot
 * When a Collision names them
 * Then nothing is published: no player fired it
 */
TEST(CollisionRules, MissileOfNoPlayerNeverHitsAPilot) {
  CollisionScene scene;
  const Entity missile = scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  const Entity ship = spawnShipFlownBy(scene, OTHER_PLAYER_NUMBER);
  const CollisionRules rules(scene.events, scene.components, true);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(Collision{.first = missile, .second = ship});
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
}

/**
 * Given a player missile and a player
 * When a Collision names them
 * Then nothing is published, because a missile does not hurt its own side
 */
TEST(CollisionRules, PlayerMissileDoesNotHitPlayer) {
  expectNoEventFor(EntityType::PlayerMissile, EntityType::Player);
}

/**
 * Given an enemy missile and an enemy
 * When a Collision names them
 * Then nothing is published
 */
TEST(CollisionRules, EnemyMissileDoesNotHitEnemy) {
  expectNoEventFor(EntityType::EnemyMissile, EntityType::Enemy);
}

/**
 * Given a missile of a player and a look item
 * When a Collision names them
 * Then nothing is published: a missile passes through an item
 */
TEST(CollisionRules, AMissileTouchingALookItemPublishesNothing) {
  expectNoEventFor(EntityType::PlayerMissile, EntityType::LookItem);
}

/**
 * Given an enemy and a look item
 * When a Collision names them
 * Then nothing is published
 */
TEST(CollisionRules, AnEnemyTouchingALookItemPublishesNothing) {
  expectNoEventFor(EntityType::Enemy, EntityType::LookItem);
}

/**
 * Given a player missile and an enemy missile
 * When a Collision names them
 * Then nothing is published
 */
TEST(CollisionRules, MissilesDoNotHitEachOther) {
  expectNoEventFor(EntityType::PlayerMissile, EntityType::EnemyMissile);
}

/**
 * Given two enemies
 * When a Collision names them
 * Then nothing is published
 */
TEST(CollisionRules, EnemiesDoNotHitEachOther) {
  expectNoEventFor(EntityType::Enemy, EntityType::Enemy);
}

/**
 * Given two players
 * When a Collision names them
 * Then nothing is published
 */
TEST(CollisionRules, PlayersDoNotHitEachOther) {
  expectNoEventFor(EntityType::Player, EntityType::Player);
}

/**
 * Given an entity with no EntityType and an enemy
 * When a Collision names them
 * Then nothing is published
 */
TEST(CollisionRules, IgnoresAnEntityWithoutAType) {
  CollisionScene scene;
  const Entity untyped = scene.entities.create();
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(Collision{.first = untyped, .second = enemy});
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
  EXPECT_TRUE(recorded.shipContacts.empty());
}

/**
 * Given a player missile destroyed after its Collision was published
 * When the bus dispatches
 * Then nothing is published, because the missile no longer exists
 */
TEST(CollisionRules, IgnoresAnEntityDestroyedBeforeDelivery) {
  CollisionScene scene;
  const Entity missile = scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);
  scene.events.publish(Collision{.first = missile, .second = enemy});

  scene.components.destroy(missile);
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
}

/**
 * Given a CollisionRules that was destroyed
 * When a Collision of a player missile and an enemy is dispatched
 * Then nothing is published, because the rules no longer listen
 */
TEST(CollisionRules, StopsListeningWhenDestroyed) {
  CollisionScene scene;
  const Entity missile = scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  Recorded recorded;
  record(scene, recorded);
  {
    const CollisionRules rules(scene.events, scene.components);
  }

  scene.events.publish(Collision{.first = missile, .second = enemy});
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
}

/**
 * Given a player missile overlapping an enemy, a CollisionSystem in a
 * scheduler and the rules
 * When the scheduler runs once and the bus dispatches
 * Then one MissileHit names the missile and the enemy, in the same tick
 */
TEST(CollisionRules, ReportsAMissileHitInTheTickItHappens) {
  CollisionScene scene;
  const Entity missile = scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  const Entity enemy =
      scene.spawn(EntityType::Enemy, CENTER_DISTANCE_OVERLAPPING, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  SystemScheduler scheduler;
  scheduler.add(std::make_unique<CollisionSystem>(scene.events));
  Recorded recorded;
  record(scene, recorded);

  scheduler.run(scene.components, SIMULATION_TICK_DURATION);
  scene.events.dispatch();

  EXPECT_EQ(recorded.missileHits, (std::vector<MissileHit>{MissileHit{
                                      .missile = missile, .target = enemy}}));
}

/**
 * Given a missile of an enemy whose sheet hits the enemies, and an enemy
 * When a Collision names them, in either order
 * Then one MissileHit is published, with the missile and the enemy as target:
 * what a missile hits comes from its sheet
 */
TEST_P(CollisionRulesInAnyOrder, AMissileHitsTheLayersItsSheetNames) {
  CollisionScene scene;
  const Entity missile = scene.spawnWith(EntityType::EnemyMissile,
                                         ENEMY_MISSILE_HITTING_ENEMIES_FILTER);
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(missile, enemy));
  scene.events.dispatch();

  EXPECT_EQ(recorded.missileHits, (std::vector<MissileHit>{MissileHit{
                                      .missile = missile, .target = enemy}}));
}

/**
 * Given a missile of a player that hits nothing, and an enemy that stops the
 * missiles of the players
 * When a Collision names them, in either order
 * Then one MissileHit is published, with the missile and the enemy as target:
 * the target naming the missile is enough
 */
TEST_P(CollisionRulesInAnyOrder, ATargetThatNamesTheMissileIsHit) {
  CollisionScene scene;
  const Entity missile = scene.spawnWith(EntityType::PlayerMissile,
                                         HARMLESS_PLAYER_MISSILE_FILTER);
  const Entity enemy =
      scene.spawnWith(EntityType::Enemy, ENEMY_STOPPING_PLAYER_MISSILES_FILTER);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(missile, enemy));
  scene.events.dispatch();

  EXPECT_EQ(recorded.missileHits, (std::vector<MissileHit>{MissileHit{
                                      .missile = missile, .target = enemy}}));
}

/**
 * Given a missile of an enemy that hits the missiles of the players, and a
 * missile of a player, which hits no missile
 * When a Collision names them, in either order
 * Then one MissileHit is published, the missile of the enemy hitting the
 * other: of two missiles, only the one that names the other is stopped
 */
TEST_P(CollisionRulesInAnyOrder, OfTwoMissilesTheOneThatNamesTheOtherHits) {
  CollisionScene scene;
  const Entity enemyMissile = scene.spawnWith(
      EntityType::EnemyMissile, ENEMY_MISSILE_HITTING_PLAYER_MISSILES_FILTER);
  const Entity playerMissile =
      scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(enemyMissile, playerMissile));
  scene.events.dispatch();

  EXPECT_EQ(recorded.missileHits,
            (std::vector<MissileHit>{
                MissileHit{.missile = enemyMissile, .target = playerMissile}}));
}

/**
 * Given a ship that hits the enemies, and an enemy that hits nothing
 * When a Collision names them, in either order
 * Then one ShipContact is published, naming the ship and the enemy
 */
TEST_P(CollisionRulesInAnyOrder, AShipTouchesAnEnemyItsOwnSheetNames) {
  CollisionScene scene;
  const Entity ship =
      scene.spawnWith(EntityType::Player, SHIP_HITTING_ENEMIES_FILTER);
  const Entity enemy =
      scene.spawnWith(EntityType::Enemy, HARMLESS_ENEMY_FILTER);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(ship, enemy));
  scene.events.dispatch();

  EXPECT_EQ(
      recorded.shipContacts,
      (std::vector<ShipContact>{ShipContact{.player = ship, .enemy = enemy}}));
}

/**
 * Given a missile of a player that hits nothing, and an enemy that hits the
 * ships only
 * When a Collision names them
 * Then nothing is published: neither sheet names the layer of the other
 */
TEST(CollisionRules, AMissileCrossesAnEnemyNeitherSheetNames) {
  CollisionScene scene;
  const Entity missile = scene.spawnWith(EntityType::PlayerMissile,
                                         HARMLESS_PLAYER_MISSILE_FILTER);
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(Collision{.first = missile, .second = enemy});
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
}

/**
 * Given a missile of a player and an enemy that lost its collision filter
 * When a Collision names them
 * Then nothing is published: an entity without a filter touches nothing
 */
TEST(CollisionRules, AnEntityWithoutACollisionFilterIsIgnored) {
  CollisionScene scene;
  const Entity missile = scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  scene.components.remove<CollisionFilter>(enemy);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(Collision{.first = missile, .second = enemy});
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
}

/**
 * Given every sheet the game ships with
 * When its collision filter is compared with the one these tests give a body
 * of its side
 * Then they are equal: the contacts judged here are the ones the game ships
 */
TEST(CollisionRules, EveryShippedSheetHasTheFilterOfItsSide) {
  for (const EntitySheet& sheet : shippedEntitySheets().all()) {
    EXPECT_EQ(sheet.collision.filter, shippedFilterOf(sheet.side))
        << sheet.name;
  }
}
