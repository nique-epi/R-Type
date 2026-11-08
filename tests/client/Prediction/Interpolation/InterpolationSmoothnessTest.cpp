#include <gtest/gtest.h>
#include "InterpolationScenarioFixtures.hpp"
#include "InterpolationTestConstants.hpp"

using rtype::client::testing::everyTwentiethState;
using rtype::client::testing::everyTwentiethStateAndTwoInARow;
using rtype::client::testing::InterpolationScenario;
using rtype::client::testing::LARGEST_SMOOTH_STEP;
using rtype::client::testing::largestStepInterpolated;
using rtype::client::testing::largestStepOfTheNewestState;
using rtype::client::testing::SMALLEST_JUMP;
using rtype::client::testing::TICKS_BETWEEN_STATES;
using rtype::client::testing::TICKS_BETWEEN_STATES_AT_TWENTY;

/**
 * Given a Bydo whose states come 10 times per second, 5 % of them lost and
 * some arriving a tick late
 * When the client shows it over 20 seconds
 * Then it never moves more than twice its own speed between two frames
 */
TEST(InterpolationSmoothness, MovesWithoutAJumpAtTenStatesPerSecond) {
  const InterpolationScenario scenario{
      .ticksBetweenStates = TICKS_BETWEEN_STATES,
      .lostStates = everyTwentiethState()};

  EXPECT_LE(largestStepInterpolated(scenario), LARGEST_SMOOTH_STEP);
}

/**
 * Given a Bydo whose states come 20 times per second, 5 % of them lost and
 * some arriving a tick late
 * When the client shows it over 10 seconds
 * Then it never moves more than twice its own speed between two frames
 */
TEST(InterpolationSmoothness, MovesWithoutAJumpAtTwentyStatesPerSecond) {
  const InterpolationScenario scenario{
      .ticksBetweenStates = TICKS_BETWEEN_STATES_AT_TWENTY,
      .lostStates = everyTwentiethState()};

  EXPECT_LE(largestStepInterpolated(scenario), LARGEST_SMOOTH_STEP);
}

/**
 * Given states 10 times per second with two lost in a row besides 5 % lost
 * When the client shows the Bydo
 * Then it still never moves more than twice its own speed between two frames,
 * since the extrapolation covers the two states
 */
TEST(InterpolationSmoothness, TwoLostStatesInARowStayWithinTheExtrapolation) {
  const InterpolationScenario scenario{
      .ticksBetweenStates = TICKS_BETWEEN_STATES,
      .lostStates = everyTwentiethStateAndTwoInARow()};

  EXPECT_LE(largestStepInterpolated(scenario), LARGEST_SMOOTH_STEP);
}

/**
 * Given the same scenario at 10 states per second with 5 % lost
 * When the client shows the Bydo where the newest state received puts it
 * Then it jumps by at least 12 units between two frames, so the bound of the
 * tests above does tell the two ways of showing it apart
 */
TEST(InterpolationSmoothness, TheNewestStateAloneJumpsInTheSameScenario) {
  const InterpolationScenario scenario{
      .ticksBetweenStates = TICKS_BETWEEN_STATES,
      .lostStates = everyTwentiethState()};

  EXPECT_GE(largestStepOfTheNewestState(scenario), SMALLEST_JUMP);
}
