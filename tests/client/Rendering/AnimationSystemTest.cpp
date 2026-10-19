#include <gtest/gtest.h>
#include <chrono>
#include "AnimationSystem.hpp"
#include "AnimationTestConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "Layer.hpp"
#include "ScreenTestConstants.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

using rtype::client::AnimationSystem;
using rtype::client::SpriteAnimation;
using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::Layer;
using rtype::game::Sprite;
using rtype::game::SpriteFrame;

namespace {

constexpr SpriteFrame NO_FRAME{.left = 0, .top = 0, .width = 0, .height = 0};
constexpr Duration TWO_AND_A_HALF_FRAMES = std::chrono::milliseconds(250);
constexpr Duration SMALL_STEP = std::chrono::milliseconds(10);
constexpr int MANY_CYCLES = 10;
constexpr const char* SHEET_ID = "sheet";

/** @brief Registries holding one entity, showing FIRST and an animation, and
 * the system that plays it. */
class AnimatedScene {
 public:
  explicit AnimatedScene(const SpriteAnimation& animation)
      : entity_(entities_.create()) {
    components_.add(
        entity_,
        Sprite{.assetId = SHEET_ID, .frame = FIRST, .layer = Layer::Entities});
    components_.add(entity_, animation);
  }

  void play(Duration elapsed) { system_.update(components_, elapsed); }

  [[nodiscard]] SpriteFrame shownFrame() {
    const Sprite* sprite = components_.get<Sprite>(entity_);
    return sprite != nullptr ? sprite->frame : NO_FRAME;
  }

  [[nodiscard]] bool isAlive() const { return entities_.isAlive(entity_); }

 private:
  EntityRegistry entities_;
  ComponentRegistry components_{entities_};
  Entity entity_;
  AnimationSystem system_;
};

}  // namespace

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
