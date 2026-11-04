#include <gtest/gtest.h>
#include "AnimatedScene.hpp"
#include "AnimationSystemTestConstants.hpp"
#include "AnimationTestConstants.hpp"
#include "ScreenTestConstants.hpp"
#include "SpriteAnimation.hpp"
#include "TimeConstants.hpp"

using rtype::client::SpriteAnimation;
using rtype::engine::Duration;

/**
 * Given an entity showing the first frame of a loop of 100 ms frames
 * When the system plays 100 ms
 * Then the sprite shows the second frame
 */
TEST(AnimationSystem, AStepShowsTheFrameItsTimeGives) {
  AnimatedScene scene(SpriteAnimation{.animation = LOOP});

  scene.play(FRAME_TIME);

  EXPECT_EQ(scene.shownFrame(), SECOND);
}

/**
 * Given two entities showing the same loop of 100 ms frames
 * When one plays 250 ms in one step and the other in 25 steps of 10 ms
 * Then both show the third frame, the one due after 250 ms
 */
TEST(AnimationSystem, ManySmallStepsLandOnTheFrameOfOneLongStep) {
  AnimatedScene oneStep(SpriteAnimation{.animation = LOOP});
  AnimatedScene smallSteps(SpriteAnimation{.animation = LOOP});

  oneStep.play(TWO_AND_A_HALF_FRAMES);
  for (Duration played{}; played < TWO_AND_A_HALF_FRAMES;
       played += SMALL_STEP) {
    smallSteps.play(SMALL_STEP);
  }

  EXPECT_EQ(oneStep.shownFrame(), THIRD);
  EXPECT_EQ(smallSteps.shownFrame(), THIRD);
}

/**
 * Given an entity whose loop starts 100 ms in, as an entity created out of
 * step
 * When the system plays no time
 * Then the sprite shows the frame due at 100 ms, the second
 */
TEST(AnimationSystem, AnAnimationStartedLateShowsTheFrameOfItsStart) {
  AnimatedScene scene(
      SpriteAnimation{.animation = LOOP, .elapsed = FRAME_TIME});

  scene.play(Duration::zero());

  EXPECT_EQ(scene.shownFrame(), SECOND);
}

/**
 * Given an effect of three 100 ms frames played once
 * When the system plays one nanosecond less than 300 ms
 * Then the effect is still alive
 */
TEST(AnimationSystem, AnEffectStaysUntilItsLastFrameHasShown) {
  AnimatedScene scene(SpriteAnimation{.animation = ONCE});

  scene.play(THREE_FRAMES_LONG - ONE_TICK);

  EXPECT_TRUE(scene.isAlive());
}

/**
 * Given an effect of three 100 ms frames played once
 * When the system plays 300 ms
 * Then the effect is destroyed
 */
TEST(AnimationSystem, AnEffectIsDestroyedOnceItsLastFrameHasShown) {
  AnimatedScene scene(SpriteAnimation{.animation = ONCE});

  scene.play(THREE_FRAMES_LONG);

  EXPECT_FALSE(scene.isAlive());
}

/**
 * Given an entity showing a loop
 * When the system plays ten cycles
 * Then the entity is still alive
 */
TEST(AnimationSystem, ALoopingEntityIsNeverDestroyed) {
  AnimatedScene scene(SpriteAnimation{.animation = LOOP});

  scene.play(THREE_FRAMES_LONG * MANY_CYCLES);

  EXPECT_TRUE(scene.isAlive());
}

/**
 * Given an entity whose animation has no frame
 * When the system plays 100 ms
 * Then the sprite keeps the frame it showed
 */
TEST(AnimationSystem, AnAnimationWithoutFramesLeavesTheSpriteAsItIs) {
  AnimatedScene scene(SpriteAnimation{.animation = WITHOUT_FRAMES});

  scene.play(FRAME_TIME);

  EXPECT_EQ(scene.shownFrame(), FIRST);
}

/**
 * Given an entity playing three frames once, a loop to follow
 * When the system plays 300 ms
 * Then the entity is alive and shows the first frame of the loop
 */
TEST(AnimationSystem, AnAnimationPlayedOnceHandsOverToWhatFollows) {
  AnimatedScene scene(SpriteAnimation{.animation = ONCE, .afterwards = LOOP});

  scene.play(THREE_FRAMES_LONG);

  EXPECT_TRUE(scene.isAlive());
  EXPECT_EQ(scene.shownFrame(), FIRST);
}

/**
 * Given an entity playing three frames once, a loop to follow, 300 ms played
 * When the system plays 100 ms more
 * Then it shows the second frame of the loop: the loop plays on its own time
 */
TEST(AnimationSystem, WhatFollowsPlaysFromItsOwnStart) {
  AnimatedScene scene(SpriteAnimation{.animation = ONCE, .afterwards = LOOP});
  scene.play(THREE_FRAMES_LONG);

  scene.play(FRAME_TIME);

  EXPECT_EQ(scene.shownFrame(), SECOND);
}

/**
 * Given an entity holding three frames
 * When the system plays ten cycles
 * Then the entity is alive and shows the last frame
 */
TEST(AnimationSystem, AHeldAnimationNeverDestroysItsEntity) {
  AnimatedScene scene(SpriteAnimation{.animation = HOLD});

  scene.play(THREE_FRAMES_LONG * MANY_CYCLES);

  EXPECT_TRUE(scene.isAlive());
  EXPECT_EQ(scene.shownFrame(), THIRD);
}
