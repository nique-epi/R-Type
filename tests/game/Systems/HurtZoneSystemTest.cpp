#include <gtest/gtest.h>
#include <vector>
#include "CollisionScene.hpp"
#include "CurrentAction.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "HurtBox.hpp"
#include "HurtZone.hpp"
#include "HurtZoneSystem.hpp"
#include "HurtZoneSystemTestConstants.hpp"
#include "ShipContact.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Entity;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::CurrentAction;
using rtype::game::EntityType;
using rtype::game::HurtBox;
using rtype::game::HurtZone;
using rtype::game::HurtZoneSystem;
using rtype::game::ShipContact;
using rtype::game::testing::ALWAYS_HURTING_BOX;
using rtype::game::testing::MOVED_ENEMY_X;
using rtype::game::testing::SHIP_APART_FROM_THE_BOX_X;
using rtype::game::testing::SHIP_IN_THE_MOVED_BOX_X;
using rtype::game::testing::SHIP_INSIDE_THE_BOX_X;
using rtype::game::testing::SHIP_TOUCHING_THE_BOX_X;
using rtype::game::testing::SIGNAL_OF_ATTACK_ONE;
using rtype::game::testing::STRIKE_BOX;
using rtype::game::testing::STRIKE_OF_ATTACK_ONE;
using rtype::game::testing::STRIKE_OF_ATTACK_TWO;

namespace {

/** @returns Every ShipContact the system publishes for @p scene in one
 * update. */
std::vector<ShipContact> contactsOf(CollisionScene& scene) {
  HurtZoneSystem system(scene.events);
  std::vector<ShipContact> received;
  const auto handle = scene.events.subscribe<ShipContact>(
      [&received](const ShipContact& contact) { received.push_back(contact); });
  system.update(scene.components, SIMULATION_TICK_DURATION);
  scene.events.dispatch();
  scene.events.unsubscribe(handle);
  return received;
}

/** @returns An enemy at (@p centerX, 0) whose hurt zone holds @p boxes. */
Entity enemyWith(CollisionScene& scene, float centerX,
                 const std::vector<HurtBox>& boxes) {
  const Entity enemy =
      scene.spawnWithoutCollisionBox(EntityType::Enemy, centerX, 0.0F);
  scene.components.add(enemy, HurtZone{.boxes = boxes});
  return enemy;
}

}  // namespace

/**
 * Given an enemy with a box that hurts all the time and a ship inside it
 * When the hurt zone system updates and the bus dispatches
 * Then one ShipContact names the ship and the enemy
 */
TEST(HurtZoneSystem, AShipInsideABoxThatAlwaysHurtsIsHurt) {
  CollisionScene scene;
  const Entity enemy = enemyWith(scene, 0.0F, {ALWAYS_HURTING_BOX});
  const Entity ship =
      scene.spawn(EntityType::Player, SHIP_INSIDE_THE_BOX_X, 0.0F);

  const std::vector<ShipContact> received = contactsOf(scene);

  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(received.front().player, ship);
  EXPECT_EQ(received.front().enemy, enemy);
}

/**
 * Given an enemy with a box and a ship apart from it
 * When the hurt zone system updates and the bus dispatches
 * Then no ShipContact is delivered
 */
TEST(HurtZoneSystem, AShipApartFromEveryBoxIsNotHurt) {
  CollisionScene scene;
  enemyWith(scene, 0.0F, {ALWAYS_HURTING_BOX});
  scene.spawn(EntityType::Player, SHIP_APART_FROM_THE_BOX_X, 0.0F);

  EXPECT_TRUE(contactsOf(scene).empty());
}

/**
 * Given an enemy with a box and a ship whose edge only touches the box
 * When the hurt zone system updates and the bus dispatches
 * Then no ShipContact is delivered: sharing an edge is not overlapping
 */
TEST(HurtZoneSystem, AShipThatOnlyTouchesTheEdgeOfABoxIsNotHurt) {
  CollisionScene scene;
  enemyWith(scene, 0.0F, {ALWAYS_HURTING_BOX});
  scene.spawn(EntityType::Player, SHIP_TOUCHING_THE_BOX_X, 0.0F);

  EXPECT_TRUE(contactsOf(scene).empty());
}

/**
 * Given an enemy moved to x 100 whose box is offset 20 units to its right
 * When the hurt zone system updates with a ship at x 120 and one at x 100
 * Then only the ship in the box, at x 120, is hurt
 */
TEST(HurtZoneSystem, TheBoxFollowsTheEnemyWithItsOffset) {
  CollisionScene scene;
  enemyWith(scene, MOVED_ENEMY_X, {ALWAYS_HURTING_BOX});
  const Entity inTheBox =
      scene.spawn(EntityType::Player, SHIP_IN_THE_MOVED_BOX_X, 0.0F);
  scene.spawn(EntityType::Player, MOVED_ENEMY_X, 0.0F);

  const std::vector<ShipContact> received = contactsOf(scene);

  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(received.front().player, inTheBox);
}

/**
 * Given an enemy idle, with a box active in the strike of attack 1, and a ship
 * inside it
 * When the hurt zone system updates and the bus dispatches
 * Then no ShipContact is delivered
 */
TEST(HurtZoneSystem, ABoxOfAnActionDoesNotHurtAnIdleEnemy) {
  CollisionScene scene;
  const Entity enemy = enemyWith(scene, 0.0F, {STRIKE_BOX});
  scene.components.add(enemy, CurrentAction{});
  scene.spawn(EntityType::Player, SHIP_INSIDE_THE_BOX_X, 0.0F);

  EXPECT_TRUE(contactsOf(scene).empty());
}

/**
 * Given an enemy with no CurrentAction at all, with a box active in a strike
 * When the hurt zone system updates with a ship inside the box
 * Then no ShipContact is delivered
 */
TEST(HurtZoneSystem, ABoxOfAnActionDoesNotHurtAnEnemyWithoutCurrentAction) {
  CollisionScene scene;
  enemyWith(scene, 0.0F, {STRIKE_BOX});
  scene.spawn(EntityType::Player, SHIP_INSIDE_THE_BOX_X, 0.0F);

  EXPECT_TRUE(contactsOf(scene).empty());
}

/**
 * Given an enemy in the signal of attack 1 and one in the strike of attack 2,
 * each with a box active in the strike of attack 1, and a ship inside the box
 * of each
 * When the hurt zone system updates
 * Then no ShipContact is delivered: neither the phase nor the attack matches
 */
TEST(HurtZoneSystem, ABoxHurtsOnlyInTheAttackAndThePhaseItNames) {
  CollisionScene scene;
  const Entity otherPhase = enemyWith(scene, 0.0F, {STRIKE_BOX});
  scene.components.add(otherPhase, CurrentAction{.key = SIGNAL_OF_ATTACK_ONE});
  scene.spawn(EntityType::Player, SHIP_INSIDE_THE_BOX_X, 0.0F);
  const Entity otherAttack = enemyWith(scene, MOVED_ENEMY_X, {STRIKE_BOX});
  scene.components.add(otherAttack, CurrentAction{.key = STRIKE_OF_ATTACK_TWO});
  scene.spawn(EntityType::Player, SHIP_IN_THE_MOVED_BOX_X, 0.0F);

  EXPECT_TRUE(contactsOf(scene).empty());
}

/**
 * Given an enemy in the strike of attack 1 with a box active in that strike,
 * and a ship inside it
 * When the hurt zone system updates and the bus dispatches
 * Then one ShipContact names the ship and the enemy
 */
TEST(HurtZoneSystem, ABoxHurtsInTheActionItNames) {
  CollisionScene scene;
  const Entity enemy = enemyWith(scene, 0.0F, {STRIKE_BOX});
  scene.components.add(enemy, CurrentAction{.key = STRIKE_OF_ATTACK_ONE});
  const Entity ship =
      scene.spawn(EntityType::Player, SHIP_INSIDE_THE_BOX_X, 0.0F);

  const std::vector<ShipContact> received = contactsOf(scene);

  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(received.front().player, ship);
  EXPECT_EQ(received.front().enemy, enemy);
}

/**
 * Given an enemy with two boxes that both overlap one ship
 * When the hurt zone system updates and the bus dispatches
 * Then one ShipContact is delivered, not two
 */
TEST(HurtZoneSystem, TwoBoxesOnOneShipPublishOneContact) {
  CollisionScene scene;
  enemyWith(scene, 0.0F, {ALWAYS_HURTING_BOX, ALWAYS_HURTING_BOX});
  scene.spawn(EntityType::Player, SHIP_INSIDE_THE_BOX_X, 0.0F);

  EXPECT_EQ(contactsOf(scene).size(), 1U);
}

/**
 * Given an enemy with a box and a missile of a player inside it
 * When the hurt zone system updates and the bus dispatches
 * Then no ShipContact is delivered: only ships are hurt
 */
TEST(HurtZoneSystem, OnlyAShipIsHurt) {
  CollisionScene scene;
  enemyWith(scene, 0.0F, {ALWAYS_HURTING_BOX});
  scene.spawn(EntityType::PlayerMissile, SHIP_INSIDE_THE_BOX_X, 0.0F);

  EXPECT_TRUE(contactsOf(scene).empty());
}

/**
 * Given an enemy with no HurtZone and a ship at its center
 * When the hurt zone system updates and the bus dispatches
 * Then no ShipContact is delivered: its body is CollisionSystem's business
 */
TEST(HurtZoneSystem, AnEnemyWithoutAHurtZoneHurtsNothing) {
  CollisionScene scene;
  scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  scene.spawn(EntityType::Player, 0.0F, 0.0F);

  EXPECT_TRUE(contactsOf(scene).empty());
}
