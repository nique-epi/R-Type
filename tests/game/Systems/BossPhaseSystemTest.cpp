#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "BossHealthChanged.hpp"
#include "BossPhaseSystemFixture.hpp"
#include "BossPhaseSystemTestConstants.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStarted.hpp"
#include "EnemyPhase.hpp"

using rtype::game::BossHealthChanged;
using rtype::game::CurrentAction;
using rtype::game::EnemyActionStarted;
using rtype::game::EnemyPhase;
using rtype::game::testing::ARRIVAL_ENDED;
using rtype::game::testing::ARRIVING_BOSS;
using rtype::game::testing::BossPhaseSystemFixture;
using rtype::game::testing::FIGHTING_BOSS;
using rtype::game::testing::PHASED_BOSS_AT_ITS_RAGE;
using rtype::game::testing::PHASED_BOSS_BELOW_ITS_RAGE;
using rtype::game::testing::PHASED_BOSS_FULL_HEALTH;
using rtype::game::testing::PHASED_BOSS_ID;
using rtype::game::testing::PHASED_BOSS_RAGE_ATTACK;
using rtype::game::testing::PHASED_BOSS_SLIDE;

/**
 * Given a boss whose arrival just ended, with its full health
 * When the boss phase system runs
 * Then its fight is announced with its full health, calm
 */
TEST_F(BossPhaseSystemFixture, AnnouncesTheFightOnceTheArrivalIsOver) {
  addBoss(ARRIVING_BOSS, PHASED_BOSS_FULL_HEALTH,
          CurrentAction{.key = std::nullopt, .ended = ARRIVAL_ENDED});

  run();

  const std::vector<BossHealthChanged> expected{
      {.networkId = PHASED_BOSS_ID,
       .points = PHASED_BOSS_FULL_HEALTH,
       .full = PHASED_BOSS_FULL_HEALTH,
       .enraged = false}};
  EXPECT_EQ(healths(), expected);
  EXPECT_TRUE(boss().fighting);
}

/**
 * Given a boss whose arrival just ended
 * When the boss phase system runs twice
 * Then its fight is announced once
 */
TEST_F(BossPhaseSystemFixture, AnnouncesTheFightOnlyOnce) {
  addBoss(ARRIVING_BOSS, PHASED_BOSS_FULL_HEALTH,
          CurrentAction{.key = std::nullopt, .ended = ARRIVAL_ENDED});

  run();
  run();

  EXPECT_EQ(healths().size(), 1U);
}

/**
 * Given a boss in its fight with 119 of its 240 points, below its 120
 * When the boss phase system runs
 * Then it is enraged
 */
TEST_F(BossPhaseSystemFixture, EnragesABossBelowItsThreshold) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_BELOW_ITS_RAGE,
          CurrentAction{.key = std::nullopt, .ended = ARRIVAL_ENDED});

  run();

  EXPECT_TRUE(boss().enraged);
}

/**
 * Given a boss in its fight below its rage threshold
 * When the boss phase system runs
 * Then its rage attack 9 is announced, in the phase Enraged
 */
TEST_F(BossPhaseSystemFixture, AnnouncesItsRageAttackInThePhaseEnraged) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_BELOW_ITS_RAGE,
          CurrentAction{.key = std::nullopt, .ended = ARRIVAL_ENDED});

  run();

  const std::vector<EnemyActionStarted> expected{
      {.networkId = PHASED_BOSS_ID,
       .attack = PHASED_BOSS_RAGE_ATTACK,
       .phase = EnemyPhase::Enraged}};
  EXPECT_EQ(startedActions(), expected);
}

/**
 * Given a boss in its fight with 119 of its 240 points
 * When the boss phase system runs
 * Then its health is announced, enraged
 */
TEST_F(BossPhaseSystemFixture, AnnouncesTheRageWithItsHealth) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_BELOW_ITS_RAGE,
          CurrentAction{.key = std::nullopt, .ended = ARRIVAL_ENDED});

  run();

  const std::vector<BossHealthChanged> expected{
      {.networkId = PHASED_BOSS_ID,
       .points = PHASED_BOSS_BELOW_ITS_RAGE,
       .full = PHASED_BOSS_FULL_HEALTH,
       .enraged = true}};
  EXPECT_EQ(healths(), expected);
}

/**
 * Given a boss in its fight below its rage threshold
 * When the boss phase system runs twice
 * Then its rage attack is announced once
 */
TEST_F(BossPhaseSystemFixture, EnragesItOnlyOnce) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_BELOW_ITS_RAGE,
          CurrentAction{.key = std::nullopt, .ended = ARRIVAL_ENDED});

  run();
  run();

  EXPECT_EQ(startedActions().size(), 1U);
}

/**
 * Given a boss still sliding in, below its rage threshold
 * When the boss phase system runs
 * Then it is neither fighting nor enraged, and nothing is announced
 */
TEST_F(BossPhaseSystemFixture, NeverEnragesDuringTheArrival) {
  addBoss(ARRIVING_BOSS, PHASED_BOSS_BELOW_ITS_RAGE,
          CurrentAction{.key = PHASED_BOSS_SLIDE});

  run();

  EXPECT_FALSE(boss().enraged);
  EXPECT_FALSE(boss().fighting);
  EXPECT_TRUE(healths().empty());
  EXPECT_TRUE(startedActions().empty());
}

/**
 * Given a boss in its fight with exactly 120 of its 240 points, its
 * threshold
 * When the boss phase system runs
 * Then it stays calm: it enrages below its threshold, not at it
 */
TEST_F(BossPhaseSystemFixture, LeavesABossAboveItsThresholdCalm) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_AT_ITS_RAGE,
          CurrentAction{.key = std::nullopt, .ended = ARRIVAL_ENDED});

  run();

  EXPECT_FALSE(boss().enraged);
  EXPECT_TRUE(startedActions().empty());
}
