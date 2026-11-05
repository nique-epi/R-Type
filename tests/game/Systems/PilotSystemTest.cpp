#include <gtest/gtest.h>
#include <cmath>
#include <cstddef>
#include "BehaviourConstants.hpp"
#include "BehaviourTestConstants.hpp"
#include "Entity.hpp"
#include "Intent.hpp"
#include "Pilot.hpp"
#include "PilotSystemFixture.hpp"
#include "PilotSystemTestConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"
#include "WaveEntryPosition.hpp"

using rtype::engine::Entity;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::Intent;
using rtype::game::Pilot;
using rtype::game::Position;
using rtype::game::STRAIGHT_BEHAVIOUR_NAME;
using rtype::game::waveEntryPosition;
using rtype::game::WEAVE_BEHAVIOUR_NAME;
using rtype::game::testing::BODY_START_X;
using rtype::game::testing::DRIFTER_X_AFTER_A_SECOND;
using rtype::game::testing::HALF_PERIOD;
using rtype::game::testing::HEIGHT_TOLERANCE_UNITS;
using rtype::game::testing::LEFT_HEADING_SETTINGS;
using rtype::game::testing::LOWEST_WAVE_CENTER_Y;
using rtype::game::testing::PilotSystemFixture;
using rtype::game::testing::QUARTER_PERIOD;
using rtype::game::testing::RIGHT_HEADING_SETTINGS;
using rtype::game::testing::RIGHTWARD_X_AFTER_A_SECOND;
using rtype::game::testing::ROW_START;
using rtype::game::testing::ROW_Y;
using rtype::game::testing::TICKS_IN_QUARTER_PERIOD;
using rtype::game::testing::WAVE_BOTTOM_Y;
using rtype::game::testing::WEAVER_SETTINGS;

/**
 * Given a straight body on the row at 300, at x 800
 * When one second of simulation time passes
 * Then it is still on its row, 240 units to the left
 */
TEST_F(PilotSystemFixture, AStraightBodyCrossesLeftOnItsRow) {
  const Entity body =
      addBody(STRAIGHT_BEHAVIOUR_NAME, LEFT_HEADING_SETTINGS, ROW_START);

  run(HALF_PERIOD);

  EXPECT_FLOAT_EQ(positionOf(body).x, DRIFTER_X_AFTER_A_SECOND);
  EXPECT_FLOAT_EQ(positionOf(body).y, ROW_Y);
}

/**
 * Given a straight body on the row at 300, at x 800, whose sheet heads it
 * right
 * When one second of simulation time passes
 * Then it is 240 units to the right: the settings of its pilot reach its
 * behaviour
 */
TEST_F(PilotSystemFixture, TheSettingsOfThePilotReachItsBehaviour) {
  const Entity body =
      addBody(STRAIGHT_BEHAVIOUR_NAME, RIGHT_HEADING_SETTINGS, ROW_START);

  run(HALF_PERIOD);

  EXPECT_FLOAT_EQ(positionOf(body).x, RIGHTWARD_X_AFTER_A_SECOND);
}

/**
 * Given a weaving body on the row at 300
 * When a quarter of the 2 second period has passed
 * Then it is 48 units below its row, at the bottom of its weave
 */
TEST_F(PilotSystemFixture, AWeavingBodyReachesTheBottomOfItsWeaveAtAQuarter) {
  const Entity body = addBody(WEAVE_BEHAVIOUR_NAME, WEAVER_SETTINGS, ROW_START);

  run(QUARTER_PERIOD);

  EXPECT_NEAR(positionOf(body).y, WAVE_BOTTOM_Y, HEIGHT_TOLERANCE_UNITS);
}

/**
 * Given a weaving body on the row at 300
 * When half of the period has passed, a quarter period at a time
 * Then it is back on its row
 */
TEST_F(PilotSystemFixture, AWeavingBodyIsBackOnItsRowAtHalfThePeriod) {
  const Entity body = addBody(WEAVE_BEHAVIOUR_NAME, WEAVER_SETTINGS, ROW_START);

  run(QUARTER_PERIOD);
  run(QUARTER_PERIOD);

  EXPECT_NEAR(positionOf(body).y, ROW_Y, HEIGHT_TOLERANCE_UNITS);
}

/**
 * Given two weaving bodies on the same row
 * When one gets a quarter period in one step and the other the same quarter
 * period in 30 simulation ticks
 * Then they are at the same height: the weave does not depend on the step
 */
TEST_F(PilotSystemFixture, TheWeaveDoesNotDependOnTheStep) {
  const Entity oneStep =
      addBody(WEAVE_BEHAVIOUR_NAME, WEAVER_SETTINGS, ROW_START);
  run(QUARTER_PERIOD);
  const float oneStepHeight = positionOf(oneStep).y;
  const Entity manySteps =
      addBody(WEAVE_BEHAVIOUR_NAME, WEAVER_SETTINGS, ROW_START);
  components().destroy(oneStep);

  for (std::size_t tick = 0; tick < TICKS_IN_QUARTER_PERIOD; ++tick) {
    run(SIMULATION_TICK_DURATION);
  }

  EXPECT_NEAR(positionOf(manySteps).y, oneStepHeight, HEIGHT_TOLERANCE_UNITS);
}

/**
 * Given a weaving body on the lowest row a Bydo may appear on
 * When a quarter of the period has passed, at the bottom of its weave
 * Then its center is at 920, so its whole height stays above the status band
 */
TEST_F(PilotSystemFixture, AWeaveOnTheLowestRowStaysAboveTheStatusBand) {
  const Position lowestRow = waveEntryPosition(std::nextafter(1.0F, 0.0F));
  const Entity body = addBody(WEAVE_BEHAVIOUR_NAME, WEAVER_SETTINGS, lowestRow);

  run(QUARTER_PERIOD);

  EXPECT_NEAR(positionOf(body).y, LOWEST_WAVE_CENTER_Y, HEIGHT_TOLERANCE_UNITS);
}

/**
 * Given a body with an Intent and no Pilot, as the ship of a player
 * When the pilots run
 * Then its Intent is unchanged: only a body with a Pilot is steered by them
 */
TEST_F(PilotSystemFixture, LeavesTheIntentOfABodyWithoutPilotAlone) {
  const Entity ship =
      addBody(STRAIGHT_BEHAVIOUR_NAME, LEFT_HEADING_SETTINGS, ROW_START);
  static_cast<void>(components().remove<Pilot>(ship));

  run(QUARTER_PERIOD);

  const Intent* const intent = components().get<Intent>(ship);
  ASSERT_NE(intent, nullptr);
  EXPECT_FALSE(intent->shooting);
  EXPECT_FLOAT_EQ(positionOf(ship).x, BODY_START_X);
}
