#include <gtest/gtest.h>
#include "EnemyAction.hpp"
#include "EnemyActionPlayerFixtures.hpp"
#include "EnemyActionPlayerTestConstants.hpp"
#include "EnemyScene.hpp"
#include "Playback.hpp"
#include "TimeConstants.hpp"

using rtype::client::Playback;
using rtype::client::testing::appearanceWithARage;
using rtype::client::testing::DESCRIBED_ATTACK;
using rtype::client::testing::ENEMY_ID;
using rtype::client::testing::EnemyScene;
using rtype::client::testing::ENRAGED_IDLE_FRAME;
using rtype::client::testing::IDLE_FRAME;
using rtype::client::testing::RAGE_ATTACK;
using rtype::client::testing::RAGE_FRAME_COUNT;
using rtype::client::testing::RAGE_FRAME_TIME;
using rtype::client::testing::STRIKE_ANCHOR;
using rtype::client::testing::STRIKE_FRAME_COUNT;
using rtype::client::testing::STRIKE_FRAME_TIME;
using rtype::client::testing::UNDESCRIBED_ATTACK;
using rtype::client::testing::UNKNOWN_ENEMY_ID;
using rtype::network::EnemyActionPhase;

/**
 * Given an enemy whose sheet describes a held strike with an anchor
 * When the server announces that strike
 * Then the enemy plays the strike from its first frame, held, with the anchor
 */
TEST(EnemyActionPlayer, PlaysTheAnimationOfTheAnnouncedPhase) {
  EnemyScene scene;

  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  EXPECT_EQ(scene.playing().animation.playback, Playback::Hold);
  EXPECT_EQ(scene.playing().animation.anchor, STRIKE_ANCHOR);
  EXPECT_EQ(scene.playing().animation.frameDuration, STRIKE_FRAME_TIME);
  EXPECT_EQ(scene.playing().elapsed, rtype::engine::Duration::zero());
}

/**
 * Given an enemy that was told to strike
 * When the animation system plays the whole strike, two frames of 100 ms
 * Then the enemy shows its idle frame again
 */
TEST(EnemyActionPlayer, GoesBackToIdleOnceTheStrikeHasEnded) {
  EnemyScene scene;
  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  scene.play(STRIKE_FRAME_TIME * STRIKE_FRAME_COUNT);

  EXPECT_EQ(scene.shownFrame(), IDLE_FRAME);
  EXPECT_EQ(scene.playing().animation.playback, Playback::Loop);
}

/**
 * Given an enemy that was told to strike
 * When the animation system plays 100 ms
 * Then the enemy shows the second frame of the strike
 */
TEST(EnemyActionPlayer, ShowsTheFramesOfTheStrikeInTurn) {
  EnemyScene scene;
  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  scene.play(STRIKE_FRAME_TIME);

  EXPECT_EQ(scene.shownFrame().left, 18);
}

/**
 * Given an enemy whose sheet describes only the strike of one attack
 * When the server announces another phase, then another attack
 * Then the enemy keeps its idle animation
 */
TEST(EnemyActionPlayer, KeepsTheIdleAnimationForAPhaseTheSheetLeavesOut) {
  EnemyScene scene;

  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Signal);
  scene.announce(ENEMY_ID, UNDESCRIBED_ATTACK, EnemyActionPhase::Strike);

  EXPECT_EQ(scene.playing().animation.playback, Playback::Loop);
}

/**
 * Given an enemy
 * When the server announces a strike to an entity the client does not have
 * Then the enemy keeps its idle animation
 */
TEST(EnemyActionPlayer, IgnoresAnEntityTheClientDoesNotHave) {
  EnemyScene scene;

  scene.announce(UNKNOWN_ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  EXPECT_EQ(scene.playing().animation.playback, Playback::Loop);
}

/**
 * Given an enemy whose sheet has an enraged skin and a rage
 * When the server announces its rage and the rage plays to its end
 * Then the enemy shows its enraged idle frame
 */
TEST(EnemyActionPlayer, ShowsTheEnragedIdleOnceTheRageHasPlayed) {
  EnemyScene scene{appearanceWithARage()};
  scene.announce(ENEMY_ID, RAGE_ATTACK, EnemyActionPhase::Enraged);

  scene.play(RAGE_FRAME_TIME * RAGE_FRAME_COUNT);

  EXPECT_EQ(scene.shownFrame(), ENRAGED_IDLE_FRAME);
}

/**
 * Given an enemy enraged by the server
 * When the server announces a strike that goes back to idle and it ends
 * Then the enemy shows its enraged idle frame, not its calm one
 */
TEST(EnemyActionPlayer, AnAttackOfAnEnragedEnemyEndsOnTheEnragedIdle) {
  EnemyScene scene{appearanceWithARage()};
  scene.announce(ENEMY_ID, RAGE_ATTACK, EnemyActionPhase::Enraged);
  scene.play(RAGE_FRAME_TIME * RAGE_FRAME_COUNT);
  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  scene.play(STRIKE_FRAME_TIME * STRIKE_FRAME_COUNT);

  EXPECT_EQ(scene.shownFrame(), ENRAGED_IDLE_FRAME);
}

/**
 * Given an enemy whose sheet has no enraged skin and no rage
 * When the server announces its rage
 * Then the enemy keeps showing its idle frame
 */
TEST(EnemyActionPlayer, AnEnemyWithoutAnEnragedSkinKeepsItsIdle) {
  EnemyScene scene;
  scene.announce(ENEMY_ID, RAGE_ATTACK, EnemyActionPhase::Enraged);

  scene.play(RAGE_FRAME_TIME);

  EXPECT_EQ(scene.shownFrame(), IDLE_FRAME);
}

/**
 * Given an enemy with an enraged skin, and a rage announced for another
 * entity
 * When the server announces a strike of the enemy and it ends
 * Then the enemy shows its calm idle frame: it is not enraged
 */
TEST(EnemyActionPlayer, ARageForAnotherEntityLeavesTheEnemyCalm) {
  EnemyScene scene{appearanceWithARage()};
  scene.announce(UNKNOWN_ENEMY_ID, RAGE_ATTACK, EnemyActionPhase::Enraged);
  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  scene.play(STRIKE_FRAME_TIME * STRIKE_FRAME_COUNT);

  EXPECT_EQ(scene.shownFrame(), IDLE_FRAME);
}
