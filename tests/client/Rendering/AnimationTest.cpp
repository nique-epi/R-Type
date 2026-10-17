#include <gtest/gtest.h>
#include <chrono>
#include "Animation.hpp"
#include "AnimationTestConstants.hpp"
#include "ScreenTestConstants.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

using rtype::client::Animation;
using rtype::engine::Duration;
using rtype::game::SpriteFrame;

namespace {

constexpr Duration TWO_FRAMES_LONG = std::chrono::milliseconds(200);
constexpr int MANY_CYCLES = 1000;

constexpr Animation WITHOUT_DURATION{.frames = THREE_FRAMES,
                                     .frameDuration = Duration::zero(),
                                     .playback = Animation::Playback::Loop};

}  // namespace

/**
 * Given an animation of three frames of 100 ms
 * When it has just started
 * Then it shows its first frame
 */
TEST(Animation, AtItsStartAnAnimationShowsItsFirstFrame) {
  EXPECT_EQ(LOOP.frameAt(Duration::zero()), FIRST);
}

/**
 * Given an animation of three frames of 100 ms
 * When one nanosecond less than 100 ms has passed
 * Then it still shows its first frame
 */
TEST(Animation, AFrameShowsUntilItsDurationHasPassed) {
  EXPECT_EQ(LOOP.frameAt(FRAME_TIME - ONE_TICK), FIRST);
}

/**
 * Given an animation of three frames of 100 ms
 * When exactly 100 ms have passed
 * Then it shows its second frame
 */
TEST(Animation, TheNextFrameShowsOnceTheDurationHasPassed) {
  EXPECT_EQ(LOOP.frameAt(FRAME_TIME), SECOND);
}

/**
 * Given a looping animation of three frames of 100 ms
 * When 300 ms have passed
 * Then it shows its first frame again
 */
TEST(Animation, ALoopStartsAgainAfterItsLastFrame) {
  EXPECT_EQ(LOOP.frameAt(THREE_FRAMES_LONG), FIRST);
}

/**
 * Given a looping animation of three frames of 100 ms
 * When a thousand cycles and 200 ms have passed in one go
 * Then it shows the frame due at that time, its third
 */
TEST(Animation, ALoopShowsTheFrameDueAfterManyCycles) {
  EXPECT_EQ(LOOP.frameAt((THREE_FRAMES_LONG * MANY_CYCLES) + TWO_FRAMES_LONG),
            THIRD);
}

/**
 * Given an animation of three frames played once
 * When far more than its length has passed
 * Then it shows its last frame
 */
TEST(Animation, AnAnimationPlayedOnceStaysOnItsLastFrame) {
  EXPECT_EQ(ONCE.frameAt(THREE_FRAMES_LONG * MANY_CYCLES), THIRD);
}

/**
 * Given an animation of three frames of 100 ms played once
 * When one nanosecond less than 300 ms has passed
 * Then it has not ended
 */
TEST(Animation, AnAnimationPlayedOnceHasNotEndedDuringItsLastFrame) {
  EXPECT_FALSE(ONCE.hasEnded(THREE_FRAMES_LONG - ONE_TICK));
}

/**
 * Given an animation of three frames of 100 ms played once
 * When exactly 300 ms have passed
 * Then it has ended
 */
TEST(Animation, AnAnimationPlayedOnceEndsWhenItsLastFrameHasShown) {
  EXPECT_TRUE(ONCE.hasEnded(THREE_FRAMES_LONG));
}

/**
 * Given a looping animation
 * When a thousand cycles have passed
 * Then it has not ended
 */
TEST(Animation, ALoopNeverEnds) {
  EXPECT_FALSE(LOOP.hasEnded(THREE_FRAMES_LONG * MANY_CYCLES));
}

/**
 * Given an animation of three frames of 100 ms
 * When its cycle is asked
 * Then it lasts 300 ms
 */
TEST(Animation, ACycleShowsEveryFrameOnce) {
  EXPECT_EQ(LOOP.cycle(), THREE_FRAMES_LONG);
}

/**
 * Given an animation without frames
 * When its frame is asked
 * Then it shows nothing
 */
TEST(Animation, AnAnimationWithoutFramesShowsNothing) {
  EXPECT_FALSE(WITHOUT_FRAMES.frameAt(Duration::zero()).has_value());
}

/**
 * Given an animation whose frames last no time
 * When its frame is asked
 * Then it shows nothing, instead of dividing by zero
 */
TEST(Animation, AnAnimationWithoutDurationShowsNothing) {
  EXPECT_FALSE(WITHOUT_DURATION.frameAt(FRAME_TIME).has_value());
}
