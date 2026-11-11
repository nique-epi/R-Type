#include <gtest/gtest.h>
#include "EnemyAction.hpp"
#include "EnemyActionPlayerFixtures.hpp"
#include "EnemyActionPlayerTestConstants.hpp"
#include "EnemyScene.hpp"
#include "Playback.hpp"
#include "TimeConstants.hpp"

using rtype::client::Playback;
using rtype::client::testing::appearanceWithALook;
using rtype::client::testing::BASE_LOOK_NUMBER;
using rtype::client::testing::CRACKED_LOOK_NUMBER;
using rtype::client::testing::DESCRIBED_ATTACK;
using rtype::client::testing::ENEMY_ID;
using rtype::client::testing::EnemyScene;
using rtype::client::testing::IDLE_FRAME;
using rtype::client::testing::IDLE_FRAME_TIME;
using rtype::client::testing::LOOK_FRAME;
using rtype::client::testing::LOOK_NUMBER_PAST_THE_SHEET;
using rtype::client::testing::RAGE_ATTACK;
using rtype::client::testing::RAGE_FRAME_COUNT;
using rtype::client::testing::RAGE_FRAME_TIME;
using rtype::client::testing::STRIKE_FRAME_COUNT;
using rtype::client::testing::STRIKE_FRAME_TIME;
using rtype::network::EnemyActionPhase;

/**
 * Given an idle enemy whose sheet has one look
 * When a state lists it with that look
 * Then it loops on the frame of the look
 */
TEST(ShownLooks, AnIdleEnemyTakesTheLookTheStateNames) {
  EnemyScene scene{appearanceWithALook()};

  scene.showLook(CRACKED_LOOK_NUMBER);
  scene.play(IDLE_FRAME_TIME);

  EXPECT_EQ(scene.shownFrame(), LOOK_FRAME);
  EXPECT_EQ(scene.playing().animation.playback, Playback::Loop);
}

/**
 * Given an enemy whose sheet has one look
 * When a state lists it with look 2, a number past the looks of its sheet
 * Then it keeps its idle frame
 */
TEST(ShownLooks, ANumberPastTheLooksOfTheSheetShowsTheIdle) {
  EnemyScene scene{appearanceWithALook()};

  scene.showLook(LOOK_NUMBER_PAST_THE_SHEET);
  scene.play(IDLE_FRAME_TIME);

  EXPECT_EQ(scene.shownFrame(), IDLE_FRAME);
}

/**
 * Given an enemy showing its look
 * When a state lists it with look 0
 * Then it goes back to its idle frame
 */
TEST(ShownLooks, LookZeroGivesTheIdleBack) {
  EnemyScene scene{appearanceWithALook()};
  scene.showLook(CRACKED_LOOK_NUMBER);

  scene.showLook(BASE_LOOK_NUMBER);
  scene.play(IDLE_FRAME_TIME);

  EXPECT_EQ(scene.shownFrame(), IDLE_FRAME);
}

/**
 * Given an enemy showing its look for half a frame
 * When every next state lists the same look
 * Then its loop is not started again
 */
TEST(ShownLooks, TheLookAlreadyShownChangesNothing) {
  EnemyScene scene{appearanceWithALook()};
  scene.showLook(CRACKED_LOOK_NUMBER);
  scene.play(IDLE_FRAME_TIME / 2);

  scene.showLook(CRACKED_LOOK_NUMBER);

  EXPECT_EQ(scene.playing().elapsed, IDLE_FRAME_TIME / 2);
}

/**
 * Given an enemy showing its look
 * When it plays a strike that goes back to its idle, to its end
 * Then it shows its look again, not its idle frame
 */
TEST(ShownLooks, AnActionEndsOnTheShownLook) {
  EnemyScene scene{appearanceWithALook()};
  scene.showLook(CRACKED_LOOK_NUMBER);
  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  scene.play(STRIKE_FRAME_TIME * STRIKE_FRAME_COUNT);

  EXPECT_EQ(scene.shownFrame(), LOOK_FRAME);
}

/**
 * Given an enemy playing a strike that goes back to its idle
 * When a state names its look, then the strike plays to its end
 * Then the strike was not cut, and the look follows it
 */
TEST(ShownLooks, ALookNamedDuringAnActionFollowsIt) {
  EnemyScene scene{appearanceWithALook()};
  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  scene.showLook(CRACKED_LOOK_NUMBER);
  EXPECT_EQ(scene.playing().animation.playback, Playback::Hold);
  scene.play(STRIKE_FRAME_TIME * STRIKE_FRAME_COUNT);

  EXPECT_EQ(scene.shownFrame(), LOOK_FRAME);
}

/**
 * Given an enemy enraged by the server, whose sheet draws an enraged idle
 * When a state names its look
 * Then it shows the look the server named rather than its enraged idle
 */
TEST(ShownLooks, ALookWinsOverTheEnragedIdle) {
  EnemyScene scene{appearanceWithALook()};
  scene.announce(ENEMY_ID, RAGE_ATTACK, EnemyActionPhase::Enraged);
  scene.play(RAGE_FRAME_TIME * RAGE_FRAME_COUNT);

  scene.showLook(CRACKED_LOOK_NUMBER);
  scene.play(IDLE_FRAME_TIME);

  EXPECT_EQ(scene.shownFrame(), LOOK_FRAME);
}
