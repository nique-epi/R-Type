#include <gtest/gtest.h>
#include <chrono>
#include "ProbeThrottle.hpp"
#include "ProbeThrottleFixtures.hpp"
#include "ProbeThrottleTestConstants.hpp"

using rtype::master::ProbeThrottle;

/**
 * Given a destination that got a probe
 * When another probe goes to it 999 ms later
 * Then it is held back
 */
TEST(ProbeThrottle, HoldsBackASecondProbeWithinASecond) {
  ProbeThrottle throttle;
  const auto start = std::chrono::steady_clock::time_point{};
  ASSERT_TRUE(throttle.allows(throttledDestination(THROTTLED_PORT), start));

  EXPECT_FALSE(throttle.allows(throttledDestination(THROTTLED_PORT),
                               start + ALMOST_ONE_SECOND));
}

/**
 * Given a destination that got a probe
 * When another probe goes to it a second later
 * Then it goes
 */
TEST(ProbeThrottle, LetsAProbeThroughASecondLater) {
  ProbeThrottle throttle;
  const auto start = std::chrono::steady_clock::time_point{};
  ASSERT_TRUE(throttle.allows(throttledDestination(THROTTLED_PORT), start));

  EXPECT_TRUE(throttle.allows(throttledDestination(THROTTLED_PORT),
                              start + ONE_SECOND_GAP));
}

/**
 * Given a destination that got a probe
 * When a probe goes to another port of the same address at once
 * Then it goes: each address and port has its own gap
 */
TEST(ProbeThrottle, KeepsAGapPerPort) {
  ProbeThrottle throttle;
  const auto start = std::chrono::steady_clock::time_point{};
  ASSERT_TRUE(throttle.allows(throttledDestination(THROTTLED_PORT), start));

  EXPECT_TRUE(
      throttle.allows(throttledDestination(OTHER_THROTTLED_PORT), start));
}
