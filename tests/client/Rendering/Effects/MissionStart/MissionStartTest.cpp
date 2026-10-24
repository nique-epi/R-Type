#include <gtest/gtest.h>
#include <chrono>
#include "EffectTestConstants.hpp"
#include "MissionStart.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::warpSpeedFactorAt;
using rtype::engine::Duration;

/**
 * Given the start of a game
 * When the warp is read at the start, at its top and at its end
 * Then the stars go their own speed, then 12 times as fast, then their own
 * speed again
 */
TEST(MissionStart, TheStarsReachTwelveTimesTheirSpeedThenComeBack) {
  EXPECT_NEAR(warpSpeedFactorAt(Duration::zero()), OWN_SPEED,
              WARP_FACTOR_TOLERANCE);
  EXPECT_NEAR(warpSpeedFactorAt(WARP_TOP_TIME), TOP_WARP_SPEED,
              WARP_FACTOR_TOLERANCE);
  EXPECT_NEAR(warpSpeedFactorAt(WARP_END_TIME), OWN_SPEED,
              WARP_FACTOR_TOLERANCE);
}

/**
 * Given the warp past its top
 * When it is read every millisecond until its end
 * Then the speed never grows again and never passes 12
 */
TEST(MissionStart, TheStarsOnlySlowDownAfterTheTop) {
  float previous = warpSpeedFactorAt(WARP_TOP_TIME);
  for (Duration time = WARP_TOP_TIME; time <= WARP_END_TIME;
       time += std::chrono::milliseconds(1)) {
    const float factor = warpSpeedFactorAt(time);
    EXPECT_LE(factor, previous + WARP_FACTOR_TOLERANCE);
    EXPECT_LE(factor, TOP_WARP_SPEED + WARP_FACTOR_TOLERANCE);
    previous = factor;
  }
}

/**
 * Given a time before the start of the game
 * When the warp is read
 * Then the stars go their own speed
 */
TEST(MissionStart, NoWarpBeforeTheStart) {
  EXPECT_EQ(warpSpeedFactorAt(-ONE_TICK), OWN_SPEED);
}
