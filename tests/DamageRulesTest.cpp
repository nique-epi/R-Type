#include <gtest/gtest.h>
#include <cstdint>
#include <variant>
#include <vector>
#include "CollisionScene.hpp"
#include "CurrentAction.hpp"
#include "DamageRules.hpp"
#include "DamageRulesFixtures.hpp"
#include "DamageRulesTestConstants.hpp"
#include "DamageScene.hpp"
#include "EnemyKilled.hpp"
#include "Entity.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EntityType.hpp"
#include "Fragile.hpp"
#include "Health.hpp"
#include "MissileHit.hpp"
#include "MissileOwner.hpp"
#include "NetworkIdentity.hpp"
#include "ScoreValue.hpp"

using rtype::engine::Entity;
using rtype::game::CurrentAction;
using rtype::game::DamageRules;
using rtype::game::DestructionCause;
using rtype::game::EnemyKilled;
using rtype::game::EntityDamage;
using rtype::game::EntityDestruction;
using rtype::game::EntityType;
using rtype::game::Fragile;
using rtype::game::Health;
using rtype::game::MissileHit;
using rtype::game::MissileOwner;
using rtype::game::NetworkIdentity;
using rtype::game::ScoreValue;
using rtype::game::testing::Announcement;
using rtype::game::testing::BYDO_ID;
using rtype::game::testing::BYDO_POINTS;
using rtype::game::testing::CHARGING;
using rtype::game::testing::DamageScene;
using rtype::game::testing::FIRST_PLAYER;
using rtype::game::testing::FOUR_HEALTH_POINTS;
using rtype::game::testing::killed;
using rtype::game::testing::MISSILE_ID;
using rtype::game::testing::ONE_HEALTH_POINT;
using rtype::game::testing::OTHER_BYDO_ID;
using rtype::game::testing::OTHER_MISSILE_ID;
using rtype::game::testing::removed;
using rtype::game::testing::SECOND_PLAYER;
using rtype::game::testing::SECOND_PLAYER_NUMBER;
using rtype::game::testing::SHIP_ID;
using rtype::game::testing::STUNNED;
using rtype::game::testing::THREE_HEALTH_POINTS;

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

/**
 * Given a Bydo worth points with one health point and a missile of the second
 * player
 * When the missile hits the Bydo
 * Then the second player is announced as having killed an enemy worth those
 * points
 */
TEST(DamageRules, KillingABydoWithAPlayerMissileAnnouncesWhoEarnsWhat) {
  DamageScene damage;
  const Entity missile = damage.missileOf(MISSILE_ID, SECOND_PLAYER);
  const Entity bydo = damage.bydoWorth(
      BYDO_ID, Health{.points = ONE_HEALTH_POINT}, BYDO_POINTS);

  damage.hit(missile, bydo);

  const std::vector<EnemyKilled> expected{
      EnemyKilled{.playerNumber = SECOND_PLAYER_NUMBER, .points = BYDO_POINTS}};
  EXPECT_EQ(damage.kills, expected);
}

/**
 * Given a Bydo worth points with three health points and a player missile
 * When the missile hits the Bydo and leaves it alive
 * Then nobody earns anything yet
 */
TEST(DamageRules, HitThatLeavesTheBydoAliveAnnouncesNoKill) {
  DamageScene damage;
  const Entity missile = damage.missileOf(MISSILE_ID, SECOND_PLAYER);
  const Entity bydo = damage.bydoWorth(
      BYDO_ID, Health{.points = THREE_HEALTH_POINTS}, BYDO_POINTS);

  damage.hit(missile, bydo);

  EXPECT_TRUE(damage.kills.empty());
}

/**
 * Given a Bydo worth points with one health point and a missile without owner
 * When the missile kills the Bydo
 * Then no kill is announced
 */
TEST(DamageRules, KillWithAMissileWithoutOwnerAnnouncesNoKill) {
  DamageScene damage;
  const Entity missile = damage.missile(MISSILE_ID);
  const Entity bydo = damage.bydoWorth(
      BYDO_ID, Health{.points = ONE_HEALTH_POINT}, BYDO_POINTS);

  damage.hit(missile, bydo);

  EXPECT_TRUE(damage.kills.empty());
}

/**
 * Given a Bydo worth no points with one health point and a player missile
 * When the missile kills the Bydo
 * Then no kill is announced
 */
TEST(DamageRules, KillOfATargetWorthNothingAnnouncesNoKill) {
  DamageScene damage;
  const Entity missile = damage.missileOf(MISSILE_ID, SECOND_PLAYER);
  const Entity bydo = damage.bydo(BYDO_ID, Health{.points = ONE_HEALTH_POINT});

  damage.hit(missile, bydo);

  EXPECT_TRUE(damage.kills.empty());
}

/**
 * Given two missiles of different players reaching a Bydo of one health point
 * in the same dispatch
 * When the hits are delivered
 * Then only the first player earns the points, since the Bydo dies once
 */
TEST(DamageRules, TwoMissilesOnTheSameBydoCreditOnlyTheFirst) {
  DamageScene damage;
  const Entity first = damage.missileOf(MISSILE_ID, FIRST_PLAYER);
  const Entity second = damage.missileOf(OTHER_MISSILE_ID, SECOND_PLAYER);
  const Entity bydo = damage.bydoWorth(
      BYDO_ID, Health{.points = ONE_HEALTH_POINT}, BYDO_POINTS);

  damage.scene.events.publish(MissileHit{.missile = first, .target = bydo});
  damage.scene.events.publish(MissileHit{.missile = second, .target = bydo});
  damage.scene.events.dispatch();

  const std::vector<EnemyKilled> expected{EnemyKilled{
      .playerNumber = FIRST_PLAYER.playerNumber, .points = BYDO_POINTS}};
  EXPECT_EQ(damage.kills, expected);
}

/**
 * Given a Bydo with four health points, fragile in the recover of attack 0
 * and in that recover
 * When a missile hits it
 * Then it is killed by that one hit
 */
TEST(DamageRules, AHitKillsAFragileEnemyInTheActionItIsFragileIn) {
  DamageScene damage;
  const Entity missile = damage.missile(MISSILE_ID);
  const Entity bydo =
      damage.bydo(BYDO_ID, Health{.points = FOUR_HEALTH_POINTS});
  damage.scene.components.add(bydo, Fragile{.in = STUNNED});
  damage.scene.components.add(bydo, CurrentAction{.key = STUNNED});

  damage.hit(missile, bydo);

  const std::vector<Announcement> expected{removed(MISSILE_ID),
                                           killed(BYDO_ID)};
  EXPECT_EQ(damage.announced, expected);
}

/**
 * Given a Bydo with four health points, fragile in the recover of attack 0
 * but in the strike of that attack
 * When a missile hits it
 * Then it loses one point, as any enemy
 */
TEST(DamageRules, AFragileEnemyOutOfItsActionLosesOnePoint) {
  DamageScene damage;
  const Entity missile = damage.missile(MISSILE_ID);
  const Entity bydo =
      damage.bydo(BYDO_ID, Health{.points = FOUR_HEALTH_POINTS});
  damage.scene.components.add(bydo, Fragile{.in = STUNNED});
  damage.scene.components.add(bydo, CurrentAction{.key = CHARGING});

  damage.hit(missile, bydo);

  ASSERT_NE(damage.scene.components.get<Health>(bydo), nullptr);
  EXPECT_EQ(damage.scene.components.get<Health>(bydo)->points,
            THREE_HEALTH_POINTS);
}
