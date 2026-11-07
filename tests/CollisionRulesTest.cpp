#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "Collision.hpp"
#include "CollisionRules.hpp"
#include "CollisionRulesFixtures.hpp"
#include "CollisionRulesOrderFixture.hpp"
#include "CollisionScene.hpp"
#include "CollisionSystem.hpp"
#include "CollisionTestConstants.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "MissileHit.hpp"
#include "ShipContact.hpp"
#include "SystemScheduler.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Collision;
using rtype::engine::Entity;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::engine::SystemScheduler;
using rtype::game::CollisionRules;
using rtype::game::CollisionSystem;
using rtype::game::EntityType;
using rtype::game::MissileHit;
using rtype::game::ShipContact;

/**
 * Given a player missile and an enemy
 * When a Collision names them, in either order
 * Then one MissileHit is published, with the missile and the enemy as target
 */
TEST_P(CollisionRulesOrderFixture, PlayerMissileHitsEnemy) {
  CollisionScene scene;
  const Entity missile = scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  RecordedCollisionEvents recorded;
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
TEST_P(CollisionRulesOrderFixture, EnemyMissileHitsPlayer) {
  CollisionScene scene;
  const Entity missile = scene.spawn(EntityType::EnemyMissile, 0.0F, 0.0F);
  const Entity player = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  RecordedCollisionEvents recorded;
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
TEST_P(CollisionRulesOrderFixture, PlayerTouchesEnemy) {
  CollisionScene scene;
  const Entity player = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  const Entity enemy = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  RecordedCollisionEvents recorded;
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
TEST_P(CollisionRulesOrderFixture, MissileHitsAnotherPilotWithFriendlyFire) {
  CollisionScene scene;
  const Entity missile = spawnMissileOf(scene, FIRING_PLAYER_NUMBER);
  const Entity ship = spawnShipFlownBy(scene, OTHER_PLAYER_NUMBER);
  const CollisionRules rules(scene.events, scene.components, true);
  RecordedCollisionEvents recorded;
  record(scene, recorded);

  scene.events.publish(collisionOf(missile, ship));
  scene.events.dispatch();

  EXPECT_EQ(recorded.missileHits, (std::vector<MissileHit>{MissileHit{
                                      .missile = missile, .target = ship}}));
}

INSTANTIATE_TEST_SUITE_P(BothOrders, CollisionRulesOrderFixture,
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
  RecordedCollisionEvents recorded;
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
  RecordedCollisionEvents recorded;
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
  RecordedCollisionEvents recorded;
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
  RecordedCollisionEvents recorded;
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
  RecordedCollisionEvents recorded;
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
  RecordedCollisionEvents recorded;
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
  RecordedCollisionEvents recorded;
  record(scene, recorded);

  scheduler.run(scene.components, SIMULATION_TICK_DURATION);
  scene.events.dispatch();

  EXPECT_EQ(recorded.missileHits, (std::vector<MissileHit>{MissileHit{
                                      .missile = missile, .target = enemy}}));
}
