#include <gtest/gtest.h>
#include <cstdint>
#include <variant>
#include <vector>
#include "CollisionScene.hpp"
#include "DamageRules.hpp"
#include "Entity.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EntityType.hpp"
#include "Health.hpp"
#include "MissileHit.hpp"
#include "NetworkIdentity.hpp"

using rtype::engine::Entity;
using rtype::game::DamageRules;
using rtype::game::DestructionCause;
using rtype::game::EntityDamage;
using rtype::game::EntityDestruction;
using rtype::game::EntityType;
using rtype::game::Health;
using rtype::game::MissileHit;
using rtype::game::NetworkIdentity;

namespace {

constexpr std::uint32_t MISSILE_ID = 7;
constexpr std::uint32_t OTHER_MISSILE_ID = 8;
constexpr std::uint32_t BYDO_ID = 21;
constexpr std::uint32_t OTHER_BYDO_ID = 22;
constexpr std::uint32_t SHIP_ID = 31;
constexpr int THREE_HEALTH_POINTS = 3;
constexpr int ONE_HEALTH_POINT = 1;

using Announcement = std::variant<EntityDamage, EntityDestruction>;

struct DamageScene {
  DamageScene() {
    scene.events.subscribe<EntityDamage>(
        [this](const EntityDamage& damage) { announced.emplace_back(damage); });
    scene.events.subscribe<EntityDestruction>(
        [this](const EntityDestruction& destruction) {
          announced.emplace_back(destruction);
        });
  }

  Entity missile(std::uint32_t networkId) {
    const Entity entity = scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
    scene.components.add(entity, NetworkIdentity{.networkId = networkId});
    return entity;
  }

  Entity bydo(std::uint32_t networkId, Health health) {
    const Entity entity = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
    scene.components.add(entity, NetworkIdentity{.networkId = networkId});
    scene.components.add(entity, health);
    return entity;
  }

  void hit(Entity missile, Entity target) {
    scene.events.publish(MissileHit{.missile = missile, .target = target});
    scene.events.dispatch();
  }

  CollisionScene scene;
  DamageRules rules{scene.events, scene.components};
  std::vector<Announcement> announced;
};

Announcement removed(std::uint32_t networkId) {
  return EntityDestruction{.networkId = networkId,
                           .cause = DestructionCause::Removed};
}

Announcement killed(std::uint32_t networkId) {
  return EntityDestruction{.networkId = networkId,
                           .cause = DestructionCause::Killed};
}

}  // namespace

/**
 * Given a Bydo with three health points and a missile
 * When the missile hits the Bydo
 * Then the Bydo has two points left and is announced as damaged, and the
 * missile is gone and announced as removed
 */
TEST(DamageRules, HitThatLeavesHealthDamagesTheTargetAndRemovesTheMissile) {
  DamageScene damage;
  const Entity missile = damage.missile(MISSILE_ID);
  const Entity bydo =
      damage.bydo(BYDO_ID, Health{.points = THREE_HEALTH_POINTS});

  damage.hit(missile, bydo);

  const std::vector<Announcement> expected{removed(MISSILE_ID),
                                           EntityDamage{.networkId = BYDO_ID}};
  EXPECT_EQ(damage.announced, expected);
  ASSERT_NE(damage.scene.components.get<Health>(bydo), nullptr);
  EXPECT_EQ(damage.scene.components.get<Health>(bydo)->points, 2);
  EXPECT_EQ(damage.scene.components.get<EntityType>(missile), nullptr);
}

/**
 * Given a Bydo with one health point and a missile
 * When the missile hits the Bydo
 * Then the Bydo is destroyed and announced as killed, and the missile is
 * removed
 */
TEST(DamageRules, HitThatTakesTheLastPointKillsTheTarget) {
  DamageScene damage;
  const Entity missile = damage.missile(MISSILE_ID);
  const Entity bydo = damage.bydo(BYDO_ID, Health{.points = ONE_HEALTH_POINT});

  damage.hit(missile, bydo);

  const std::vector<Announcement> expected{removed(MISSILE_ID),
                                           killed(BYDO_ID)};
  EXPECT_EQ(damage.announced, expected);
  EXPECT_EQ(damage.scene.components.get<EntityType>(bydo), nullptr);
}

/**
 * Given a Bydo with three health points
 * When three missiles hit it one after the other
 * Then the first two only damage it and the third kills it
 */
TEST(DamageRules, ThreeHitsKillABydoOfThreeHealthPoints) {
  DamageScene damage;
  const Entity bydo =
      damage.bydo(BYDO_ID, Health{.points = THREE_HEALTH_POINTS});

  damage.hit(damage.missile(MISSILE_ID), bydo);
  damage.hit(damage.missile(MISSILE_ID), bydo);
  damage.hit(damage.missile(MISSILE_ID), bydo);

  const std::vector<Announcement> expected{
      removed(MISSILE_ID), EntityDamage{.networkId = BYDO_ID},
      removed(MISSILE_ID), EntityDamage{.networkId = BYDO_ID},
      removed(MISSILE_ID), killed(BYDO_ID)};
  EXPECT_EQ(damage.announced, expected);
}

/**
 * Given a Bydo with one health point and two missiles that reach it in the
 * same dispatch
 * When both hits are handled
 * Then the Bydo is killed once, and the second missile is left alone
 */
TEST(DamageRules, TwoMissilesOnTheSameTargetKillItOnce) {
  DamageScene damage;
  const Entity firstMissile = damage.missile(MISSILE_ID);
  const Entity secondMissile = damage.missile(OTHER_MISSILE_ID);
  const Entity bydo = damage.bydo(BYDO_ID, Health{.points = ONE_HEALTH_POINT});

  damage.scene.events.publish(
      MissileHit{.missile = firstMissile, .target = bydo});
  damage.scene.events.publish(
      MissileHit{.missile = secondMissile, .target = bydo});
  damage.scene.events.dispatch();

  const std::vector<Announcement> expected{removed(MISSILE_ID),
                                           killed(BYDO_ID)};
  EXPECT_EQ(damage.announced, expected);
  EXPECT_NE(damage.scene.components.get<EntityType>(secondMissile), nullptr);
}

/**
 * Given a missile that reaches two Bydos in the same dispatch
 * When both hits are handled
 * Then only the first Bydo is hurt: the missile is gone at the second hit
 */
TEST(DamageRules, OneMissileOnTwoTargetsHurtsOnlyTheFirst) {
  DamageScene damage;
  const Entity missile = damage.missile(MISSILE_ID);
  const Entity firstBydo =
      damage.bydo(BYDO_ID, Health{.points = THREE_HEALTH_POINTS});
  const Entity secondBydo =
      damage.bydo(OTHER_BYDO_ID, Health{.points = THREE_HEALTH_POINTS});

  damage.scene.events.publish(
      MissileHit{.missile = missile, .target = firstBydo});
  damage.scene.events.publish(
      MissileHit{.missile = missile, .target = secondBydo});
  damage.scene.events.dispatch();

  const std::vector<Announcement> expected{removed(MISSILE_ID),
                                           EntityDamage{.networkId = BYDO_ID}};
  EXPECT_EQ(damage.announced, expected);
  EXPECT_EQ(damage.scene.components.get<Health>(secondBydo)->points,
            THREE_HEALTH_POINTS);
}

/**
 * Given the ship of a player, which has no health, and an enemy missile
 * When the missile hits the ship
 * Then the missile is removed and the ship is neither damaged nor destroyed
 */
TEST(DamageRules, TargetWithoutHealthIsNotHurtButTheMissileDisappears) {
  DamageScene damage;
  const Entity missile =
      damage.scene.spawn(EntityType::EnemyMissile, 0.0F, 0.0F);
  damage.scene.components.add(missile,
                              NetworkIdentity{.networkId = MISSILE_ID});
  const Entity ship = damage.scene.spawn(EntityType::Player, 0.0F, 0.0F);
  damage.scene.components.add(ship, NetworkIdentity{.networkId = SHIP_ID});

  damage.hit(missile, ship);

  const std::vector<Announcement> expected{removed(MISSILE_ID)};
  EXPECT_EQ(damage.announced, expected);
  EXPECT_NE(damage.scene.components.get<EntityType>(ship), nullptr);
}

/**
 * Given a missile and a Bydo that have no network identity
 * When the missile hits the Bydo
 * Then nothing is announced and nothing is hurt
 */
TEST(DamageRules, HitOnEntitiesThatAreNotNetworkedIsIgnored) {
  DamageScene damage;
  const Entity missile =
      damage.scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  const Entity bydo = damage.scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  damage.scene.components.add(bydo, Health{.points = THREE_HEALTH_POINTS});

  damage.hit(missile, bydo);

  EXPECT_TRUE(damage.announced.empty());
  EXPECT_EQ(damage.scene.components.get<Health>(bydo)->points,
            THREE_HEALTH_POINTS);
}
