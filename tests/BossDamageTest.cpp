#include <gtest/gtest.h>
#include <vector>
#include "BossDamageTestConstants.hpp"
#include "BossHealthChanged.hpp"
#include "CurrentAction.hpp"
#include "DamageRulesFixtures.hpp"
#include "DamageRulesTestConstants.hpp"
#include "DamageScene.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityType.hpp"
#include "Health.hpp"
#include "WorldTestConstants.hpp"

using rtype::engine::Entity;
using rtype::game::BossHealthChanged;
using rtype::game::CurrentAction;
using rtype::game::EntityType;
using rtype::game::Health;
using rtype::game::testing::Announcement;
using rtype::game::testing::ARRIVAL_ROAR;
using rtype::game::testing::ARRIVAL_SLIDE;
using rtype::game::testing::BOSS_FULL_HEALTH;
using rtype::game::testing::BOSS_HEALTH_AFTER_ONE_HIT;
using rtype::game::testing::BOSS_ID;
using rtype::game::testing::BYDO_ID;
using rtype::game::testing::CALM_BOSS;
using rtype::game::testing::DamageScene;
using rtype::game::testing::killed;
using rtype::game::testing::MISSILE_ID;
using rtype::game::testing::ONE_HEALTH_POINT;
using rtype::game::testing::removed;
using rtype::game::testing::THREE_HEALTH_POINTS;

/**
 * Given a boss sliding in, the signal of its arrival, with its full health
 * When a missile hits it
 * Then it keeps every point: no hit hurts it while it arrives
 */
TEST(BossDamage, AnArrivingBossLosesNothing) {
  DamageScene damage;
  const Entity missile = damage.missile(MISSILE_ID);
  const Entity boss =
      damage.boss(Health{.points = BOSS_FULL_HEALTH, .full = BOSS_FULL_HEALTH},
                  CALM_BOSS, CurrentAction{.key = ARRIVAL_SLIDE});

  damage.hit(missile, boss);

  ASSERT_NE(damage.scene.components.get<Health>(boss), nullptr);
  EXPECT_EQ(damage.scene.components.get<Health>(boss)->points,
            BOSS_FULL_HEALTH);
}

/**
 * Given a boss roaring, the strike of its arrival
 * When a missile hits it
 * Then the missile is gone and announced as removed, and nothing else is
 * announced
 */
TEST(BossDamage, AMissileOnAnArrivingBossIsRemoved) {
  DamageScene damage;
  const Entity missile = damage.missile(MISSILE_ID);
  const Entity boss =
      damage.boss(Health{.points = BOSS_FULL_HEALTH, .full = BOSS_FULL_HEALTH},
                  CALM_BOSS, CurrentAction{.key = ARRIVAL_ROAR});

  damage.hit(missile, boss);

  const std::vector<Announcement> expected{removed(MISSILE_ID)};
  EXPECT_EQ(damage.announced, expected);
  EXPECT_EQ(damage.scene.components.get<EntityType>(missile), nullptr);
  EXPECT_TRUE(damage.bossHealths.empty());
}

/**
 * Given a boss in its fight with its full health of 240
 * When a missile hits it
 * Then its health of 239 out of 240, calm, is announced
 */
TEST(BossDamage, AHitBossAnnouncesItsHealth) {
  DamageScene damage;
  const Entity missile = damage.missile(MISSILE_ID);
  const Entity boss =
      damage.boss(Health{.points = BOSS_FULL_HEALTH, .full = BOSS_FULL_HEALTH},
                  CALM_BOSS, CurrentAction{});

  damage.hit(missile, boss);

  const std::vector<BossHealthChanged> expected{
      {.networkId = BOSS_ID,
       .points = BOSS_HEALTH_AFTER_ONE_HIT,
       .full = BOSS_FULL_HEALTH,
       .enraged = false}};
  EXPECT_EQ(damage.bossHealths, expected);
}

/**
 * Given a boss in its fight with one point left
 * When a missile hits it
 * Then 0 points are announced, then the boss is killed
 */
TEST(BossDamage, AKilledBossAnnouncesNoPointsLeft) {
  DamageScene damage;
  const Entity missile = damage.missile(MISSILE_ID);
  const Entity boss =
      damage.boss(Health{.points = ONE_HEALTH_POINT, .full = BOSS_FULL_HEALTH},
                  CALM_BOSS, CurrentAction{});

  damage.hit(missile, boss);

  ASSERT_EQ(damage.bossHealths.size(), 1U);
  EXPECT_EQ(damage.bossHealths.front().points, 0U);
  const std::vector<Announcement> expected{removed(MISSILE_ID),
                                           killed(BOSS_ID)};
  EXPECT_EQ(damage.announced, expected);
}

/**
 * Given a Bydo that is no boss
 * When a missile hits it
 * Then no boss health is announced
 */
TEST(BossDamage, ABydoAnnouncesNoBossHealth) {
  DamageScene damage;
  const Entity missile = damage.missile(MISSILE_ID);
  const Entity bydo =
      damage.bydo(BYDO_ID, Health{.points = THREE_HEALTH_POINTS});

  damage.hit(missile, bydo);

  EXPECT_TRUE(damage.bossHealths.empty());
}
