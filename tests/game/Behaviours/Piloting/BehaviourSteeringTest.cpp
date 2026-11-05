#include <gtest/gtest.h>
#include "BehaviourSteering.hpp"
#include "BehaviourTestConstants.hpp"
#include "BehaviourView.hpp"
#include "GameException.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Duration;
using rtype::game::BehaviourView;
using rtype::game::MissingBehaviourSettingException;
using rtype::game::settingAt;
using rtype::game::verticalShareTowards;
using rtype::game::testing::BODY_TOP_SPEED;
using rtype::game::testing::QUARTER_PERIOD;
using rtype::game::testing::QUARTER_PERIOD_VERTICAL_SHARE;
using rtype::game::testing::ROW_START;
using rtype::game::testing::SECOND_SETTING;
using rtype::game::testing::SHARE_TOLERANCE;
using rtype::game::testing::THIRD_SETTING;
using rtype::game::testing::WAVE_BOTTOM_Y;
using rtype::game::testing::WEAVER_PERIOD_MS;
using rtype::game::testing::WEAVER_SETTINGS;

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
                                   .topSpeed = BODY_TOP_SPEED,
                                   .settings = {}});

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
                                   .topSpeed = BODY_TOP_SPEED,
                                   .settings = {}});

  EXPECT_EQ(share, 0.0F);
}

/**
 * Given a body whose sheet gives its weave an amplitude of 48, then a period
 * of 2000 ms
 * When its second setting is read
 * Then it is the period, 2000
 */
TEST(BehaviourSteering, ReadsASettingByItsPlace) {
  const BehaviourView view{.position = ROW_START,
                           .origin = ROW_START,
                           .age = QUARTER_PERIOD,
                           .elapsed = QUARTER_PERIOD,
                           .topSpeed = BODY_TOP_SPEED,
                           .settings = WEAVER_SETTINGS};

  EXPECT_EQ(settingAt(view, SECOND_SETTING), WEAVER_PERIOD_MS);
}

/**
 * Given a body given two settings
 * When a third one is read
 * Then it is refused, rather than read past the values of the body
 */
TEST(BehaviourSteering, RefusesASettingTheBodyWasNotGiven) {
  const BehaviourView view{.position = ROW_START,
                           .origin = ROW_START,
                           .age = QUARTER_PERIOD,
                           .elapsed = QUARTER_PERIOD,
                           .topSpeed = BODY_TOP_SPEED,
                           .settings = WEAVER_SETTINGS};

  EXPECT_THROW(static_cast<void>(settingAt(view, THIRD_SETTING)),
               MissingBehaviourSettingException);
}
