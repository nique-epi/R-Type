#include <gtest/gtest.h>
#include <vector>
#include "BossHealthChanged.hpp"
#include "BossProgress.hpp"
#include "BossSystemFixture.hpp"
#include "BossSystemTestConstants.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStarted.hpp"
#include "EnemyPhase.hpp"
#include "EntityDestruction.hpp"

using rtype::game::BossHealthChanged;
using rtype::game::BossProgress;
using rtype::game::CurrentAction;
using rtype::game::DestructionCause;
using rtype::game::EnemyActionStarted;
using rtype::game::EnemyPhase;
using rtype::game::testing::ACTION_AFTER_THE_ARRIVAL;
using rtype::game::testing::ARRIVING_BOSS;
using rtype::game::testing::BossSystemFixture;
using rtype::game::testing::FIGHTING_BOSS;
using rtype::game::testing::KILLED_BYDO_ID;
using rtype::game::testing::PHASED_BOSS_AFTER_ONE_HIT;
using rtype::game::testing::PHASED_BOSS_AT_ITS_RAGE;
using rtype::game::testing::PHASED_BOSS_BELOW_ITS_RAGE;
using rtype::game::testing::PHASED_BOSS_FULL_HEALTH;
using rtype::game::testing::PHASED_BOSS_ID;
using rtype::game::testing::PHASED_BOSS_RAGE_ATTACK;
using rtype::game::testing::PHASED_BOSS_SLIDE;

/**
 * Given a boss whose arrival just ended, with its full health
 * When the boss system runs
 * Then its fight is announced with its full health, calm
 */
TEST_F(BossSystemFixture, AnnouncesTheFightOnceTheArrivalIsOver) {
  addBoss(ARRIVING_BOSS, PHASED_BOSS_FULL_HEALTH, ACTION_AFTER_THE_ARRIVAL);

  run();

  const std::vector<BossHealthChanged> expected{
      {.networkId = PHASED_BOSS_ID,
       .points = PHASED_BOSS_FULL_HEALTH,
       .full = PHASED_BOSS_FULL_HEALTH,
       .enraged = false}};
  EXPECT_EQ(healths(), expected);
  EXPECT_EQ(boss().progress, BossProgress::Fighting);
}

/**
 * Given a boss whose arrival just ended
 * When the boss system runs twice
 * Then its fight is announced once
 */
TEST_F(BossSystemFixture, AnnouncesTheFightOnlyOnce) {
  addBoss(ARRIVING_BOSS, PHASED_BOSS_FULL_HEALTH, ACTION_AFTER_THE_ARRIVAL);

  run();
  run();

  EXPECT_EQ(healths().size(), 1U);
}

/**
 * Given a boss in its fight with 119 of its 240 points, below its 120
 * When the boss system runs
 * Then it is enraged
 */
TEST_F(BossSystemFixture, EnragesABossBelowItsThreshold) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_BELOW_ITS_RAGE, ACTION_AFTER_THE_ARRIVAL);

  run();

  EXPECT_TRUE(enraged());
}

/**
 * Given a boss in its fight below its rage threshold
 * When the boss system runs
 * Then its rage attack 9 is announced, in the phase Enraged
 */
TEST_F(BossSystemFixture, AnnouncesItsRageAttackInThePhaseEnraged) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_BELOW_ITS_RAGE, ACTION_AFTER_THE_ARRIVAL);

  run();

  const std::vector<EnemyActionStarted> expected{
      {.networkId = PHASED_BOSS_ID,
       .attack = PHASED_BOSS_RAGE_ATTACK,
       .phase = EnemyPhase::Enraged}};
  EXPECT_EQ(startedActions(), expected);
}

/**
 * Given a boss in its fight with 119 of its 240 points
 * When the boss system runs
 * Then its health is announced, enraged
 */
TEST_F(BossSystemFixture, AnnouncesTheRageWithItsHealth) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_BELOW_ITS_RAGE, ACTION_AFTER_THE_ARRIVAL);

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
 * When the boss system runs twice
 * Then its rage attack is announced once
 */
TEST_F(BossSystemFixture, EnragesItOnlyOnce) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_BELOW_ITS_RAGE, ACTION_AFTER_THE_ARRIVAL);

  run();
  run();

  EXPECT_EQ(startedActions().size(), 1U);
}

/**
 * Given a boss in its fight, told with its 240 points
 * When a hit leaves it 239 points and the boss system runs
 * Then its 239 points out of 240, calm, are told
 */
TEST_F(BossSystemFixture, TellsTheHealthAHitLeft) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_FULL_HEALTH, ACTION_AFTER_THE_ARRIVAL);
  run();

  leavePoints(PHASED_BOSS_AFTER_ONE_HIT);
  run();

  ASSERT_EQ(healths().size(), 2U);
  EXPECT_EQ(healths().back(),
            (BossHealthChanged{.networkId = PHASED_BOSS_ID,
                               .points = PHASED_BOSS_AFTER_ONE_HIT,
                               .full = PHASED_BOSS_FULL_HEALTH,
                               .enraged = false}));
}

/**
 * Given a boss in its fight, told with its 240 points
 * When it is killed
 * Then no point left out of 240 is told
 */
TEST_F(BossSystemFixture, TellsAKilledBossHasNoPointLeft) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_FULL_HEALTH, ACTION_AFTER_THE_ARRIVAL);
  run();

  destroyBoss(DestructionCause::Killed);

  ASSERT_EQ(healths().size(), 2U);
  EXPECT_EQ(healths().back(),
            (BossHealthChanged{.networkId = PHASED_BOSS_ID,
                               .points = 0,
                               .full = PHASED_BOSS_FULL_HEALTH,
                               .enraged = false}));
}

/**
 * Given a boss in its fight, told with its 240 points
 * When it is removed without being killed, as a restart from a checkpoint
 * removes it
 * Then nothing more is told
 */
TEST_F(BossSystemFixture, TellsNothingOfARemovedBoss) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_FULL_HEALTH, ACTION_AFTER_THE_ARRIVAL);
  run();

  destroyBoss(DestructionCause::Removed);

  EXPECT_EQ(healths().size(), 1U);
}

/**
 * Given a boss in its fight, told with its 240 points
 * When a Bydo that is no boss is killed
 * Then nothing more is told
 */
TEST_F(BossSystemFixture, TellsNothingWhenAnotherBodyIsKilled) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_FULL_HEALTH, ACTION_AFTER_THE_ARRIVAL);
  run();

  announceKillOf(KILLED_BYDO_ID);

  EXPECT_EQ(healths().size(), 1U);
}

/**
 * Given a boss still sliding in, which the players were told nothing of
 * When it is killed
 * Then nothing is told
 */
TEST_F(BossSystemFixture, TellsNothingOfABossKilledBeforeItsFight) {
  addBoss(ARRIVING_BOSS, PHASED_BOSS_FULL_HEALTH,
          CurrentAction{.key = PHASED_BOSS_SLIDE});
  run();

  destroyBoss(DestructionCause::Killed);

  EXPECT_TRUE(healths().empty());
}

/**
 * Given a boss still sliding in, below its rage threshold
 * When the boss system runs
 * Then it is neither fighting nor enraged, and nothing is announced
 */
TEST_F(BossSystemFixture, NeverEnragesDuringTheArrival) {
  addBoss(ARRIVING_BOSS, PHASED_BOSS_BELOW_ITS_RAGE,
          CurrentAction{.key = PHASED_BOSS_SLIDE});

  run();

  EXPECT_FALSE(enraged());
  EXPECT_EQ(boss().progress, BossProgress::Arriving);
  EXPECT_TRUE(healths().empty());
  EXPECT_TRUE(startedActions().empty());
}

/**
 * Given a boss in its fight with exactly 120 of its 240 points, its
 * threshold
 * When the boss system runs
 * Then it stays calm: it enrages below its threshold, not at it
 */
TEST_F(BossSystemFixture, LeavesABossAboveItsThresholdCalm) {
  addBoss(FIGHTING_BOSS, PHASED_BOSS_AT_ITS_RAGE, ACTION_AFTER_THE_ARRIVAL);

  run();

  EXPECT_FALSE(enraged());
  EXPECT_TRUE(startedActions().empty());
}
