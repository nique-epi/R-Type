#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "Collision.hpp"
#include "CollisionRules.hpp"
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

namespace {

struct Recorded {
  std::vector<MissileHit> missileHits;
  std::vector<ShipContact> shipContacts;
};

void record(CollisionScene& scene, Recorded& recorded) {
  scene.events.subscribe<MissileHit>([&recorded](const MissileHit& hit) {
    recorded.missileHits.push_back(hit);
  });
  scene.events.subscribe<ShipContact>([&recorded](const ShipContact& contact) {
    recorded.shipContacts.push_back(contact);
  });
}

class CollisionRulesInAnyOrder : public ::testing::TestWithParam<bool> {
 protected:
  [[nodiscard]] static Collision collisionOf(Entity entity, Entity other) {
    return GetParam() ? Collision{.first = other, .second = entity}
                      : Collision{.first = entity, .second = other};
  }
};

void expectNoEventFor(EntityType firstType, EntityType secondType) {
  CollisionScene scene;
  const Entity first = scene.spawn(firstType, 0.0F, 0.0F);
  const Entity second = scene.spawn(secondType, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(Collision{.first = first, .second = second});
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
  EXPECT_TRUE(recorded.shipContacts.empty());
}

}  // namespace

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

INSTANTIATE_TEST_SUITE_P(BothOrders, CollisionRulesInAnyOrder,
                         ::testing::Bool());

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
