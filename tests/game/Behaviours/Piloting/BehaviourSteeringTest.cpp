#include <gtest/gtest.h>
#include "BehaviourSteering.hpp"
#include "BehaviourTestConstants.hpp"
#include "BehaviourView.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Duration;
using rtype::game::BehaviourView;
using rtype::game::verticalShareTowards;
using rtype::game::testing::BODY_TOP_SPEED;
using rtype::game::testing::QUARTER_PERIOD;
using rtype::game::testing::QUARTER_PERIOD_VERTICAL_SHARE;
using rtype::game::testing::ROW_START;
using rtype::game::testing::SHARE_TOLERANCE;
using rtype::game::testing::WAVE_BOTTOM_Y;

/**
 * Given a body on the row at 300, at 240 units per second, and a step of half
 * a second
 * When it is steered towards a height 48 units lower
 * Then it is asked for the share that lands there: 48 / 120
 */
TEST(BehaviourSteering, AsksForTheShareThatLandsOnTheHeight) {
  const float share = verticalShareTowards(
      WAVE_BOTTOM_Y, BehaviourView{.position = ROW_START,
                                   .origin = ROW_START,
                                   .age = QUARTER_PERIOD,
                                   .elapsed = QUARTER_PERIOD,
                                   .topSpeed = BODY_TOP_SPEED});

  EXPECT_NEAR(share, QUARTER_PERIOD_VERTICAL_SHARE, SHARE_TOLERANCE);
}

/**
 * Given a body on its row
 * When it is steered towards another height for a step of no time
 * Then it is asked for nothing, since it cannot move during that step
 */
TEST(BehaviourSteering, AsksForNothingDuringAStepOfNoTime) {
  const float share = verticalShareTowards(
      WAVE_BOTTOM_Y, BehaviourView{.position = ROW_START,
                                   .origin = ROW_START,
                                   .age = QUARTER_PERIOD,
                                   .elapsed = Duration::zero(),
                                   .topSpeed = BODY_TOP_SPEED});

  EXPECT_EQ(share, 0.0F);
}
