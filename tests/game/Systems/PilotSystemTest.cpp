#include <gtest/gtest.h>
#include <cmath>
#include <cstddef>
#include <optional>
#include "ActionRequestingBehaviour.hpp"
#include "BehaviourConstants.hpp"
#include "BehaviourTestConstants.hpp"
#include "BehaviourView.hpp"
#include "CollisionBox.hpp"
#include "Entity.hpp"
#include "Intent.hpp"
#include "Pilot.hpp"
#include "PilotSystemFixture.hpp"
#include "PilotSystemTestConstants.hpp"
#include "Position.hpp"
#include "SteadyMotion.hpp"
#include "TimeConstants.hpp"
#include "ViewRecordingBehaviour.hpp"
#include "WaveEntryPosition.hpp"

using rtype::engine::Entity;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::BehaviourView;
using rtype::game::CollisionBox;
using rtype::game::Intent;
using rtype::game::Pilot;
using rtype::game::Position;
using rtype::game::SteadyMotion;
using rtype::game::STRAIGHT_BEHAVIOUR_NAME;
using rtype::game::waveEntryPosition;
using rtype::game::WEAVE_BEHAVIOUR_NAME;
using rtype::game::testing::ActionRequestingBehaviour;
using rtype::game::testing::ASKED_BOX;
using rtype::game::testing::ASKED_TOP_SPEED;
using rtype::game::testing::BODY_START_X;
using rtype::game::testing::DRIFTER_X_AFTER_A_SECOND;
using rtype::game::testing::FAR_SHIP;
using rtype::game::testing::HALF_PERIOD;
using rtype::game::testing::HEIGHT_TOLERANCE_UNITS;
using rtype::game::testing::HIT_ONCE;
using rtype::game::testing::JUMP_TARGET;
using rtype::game::testing::LEFT_HEADING_SETTINGS;
using rtype::game::testing::LOWEST_WAVE_CENTER_Y;
using rtype::game::testing::NEAR_SHIP;
using rtype::game::testing::PilotSystemFixture;
using rtype::game::testing::QUARTER_PERIOD;
using rtype::game::testing::RIGHT_HEADING_SETTINGS;
using rtype::game::testing::RIGHTWARD_X_AFTER_A_SECOND;
using rtype::game::testing::ROW_START;
using rtype::game::testing::ROW_Y;
using rtype::game::testing::STARTING_BOX;
using rtype::game::testing::STARTING_TOP_SPEED;
using rtype::game::testing::TICKS_IN_QUARTER_PERIOD;
using rtype::game::testing::UNTOUCHED;
using rtype::game::testing::ViewRecordingBehaviour;
using rtype::game::testing::WATCHING_ENEMY_NETWORK_ID;
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

/**
 * Given an enemy at the origin, a ship 100 units to its right and another 500
 * units below it
 * When the pilots run once
 * Then its behaviour sees the ship 100 units away
 */
TEST_F(PilotSystemFixture, ABehaviourSeesTheNearestShip) {
  std::optional<BehaviourView> seen;
  const ViewRecordingBehaviour behaviour{seen};
  static_cast<void>(addEnemy(behaviour, WATCHING_ENEMY_NETWORK_ID));
  addShip(FAR_SHIP);
  addShip(NEAR_SHIP);

  run(SIMULATION_TICK_DURATION);

  ASSERT_TRUE(seen.has_value());
  const std::optional<Position> nearest =
      seen.value_or(BehaviourView{}).nearestShip;
  ASSERT_TRUE(nearest.has_value());
  const Position found = nearest.value_or(Position{});
  EXPECT_EQ(found.x, NEAR_SHIP.x);
  EXPECT_EQ(found.y, NEAR_SHIP.y);
}

/**
 * Given an enemy and no ship in play
 * When the pilots run once
 * Then its behaviour sees no ship
 */
TEST_F(PilotSystemFixture, ABehaviourSeesNoShipWhileNoneIsInPlay) {
  std::optional<BehaviourView> seen;
  const ViewRecordingBehaviour behaviour{seen};
  static_cast<void>(addEnemy(behaviour, WATCHING_ENEMY_NETWORK_ID));

  run(SIMULATION_TICK_DURATION);

  ASSERT_TRUE(seen.has_value());
  EXPECT_FALSE(seen.value_or(BehaviourView{}).nearestShip.has_value());
}

/**
 * Given an enemy that appeared with three points and has two left
 * When the pilots run once
 * Then its behaviour sees it wounded
 */
TEST_F(PilotSystemFixture, ABehaviourSeesItsBodyWounded) {
  std::optional<BehaviourView> seen;
  const ViewRecordingBehaviour behaviour{seen};
  const Entity enemy = addEnemy(behaviour, WATCHING_ENEMY_NETWORK_ID);
  components().add(enemy, HIT_ONCE);

  run(SIMULATION_TICK_DURATION);

  ASSERT_TRUE(seen.has_value());
  EXPECT_TRUE(seen.value_or(BehaviourView{}).wounded);
}

/**
 * Given an enemy that still has the three points it appeared with
 * When the pilots run once
 * Then its behaviour does not see it wounded
 */
TEST_F(PilotSystemFixture, ABehaviourSeesAnUntouchedBodyUnwounded) {
  std::optional<BehaviourView> seen;
  const ViewRecordingBehaviour behaviour{seen};
  const Entity enemy = addEnemy(behaviour, WATCHING_ENEMY_NETWORK_ID);
  components().add(enemy, UNTOUCHED);

  run(SIMULATION_TICK_DURATION);

  ASSERT_TRUE(seen.has_value());
  EXPECT_FALSE(seen.value_or(BehaviourView{}).wounded);
}

/**
 * Given an enemy whose behaviour asks it to jump to (640, 700)
 * When the pilots run once
 * Then the enemy is at (640, 700)
 */
TEST_F(PilotSystemFixture, ABodyJumpsWhereItsBehaviourAsks) {
  const ActionRequestingBehaviour behaviour{
      Intent{.steered = true, .jumpsTo = JUMP_TARGET}};
  const Entity enemy = addEnemy(behaviour, WATCHING_ENEMY_NETWORK_ID);

  run(SIMULATION_TICK_DURATION);

  EXPECT_EQ(positionOf(enemy).x, JUMP_TARGET.x);
  EXPECT_EQ(positionOf(enemy).y, JUMP_TARGET.y);
}

/**
 * Given an enemy with a collision box whose behaviour asks for another box
 * When the pilots run once
 * Then the enemy has the box it asked for
 */
TEST_F(PilotSystemFixture, ABodyTakesTheCollisionBoxItsBehaviourAsks) {
  const ActionRequestingBehaviour behaviour{
      Intent{.steered = true, .resizesTo = ASKED_BOX}};
  const Entity enemy = addEnemy(behaviour, WATCHING_ENEMY_NETWORK_ID);
  components().add(enemy, STARTING_BOX);

  run(SIMULATION_TICK_DURATION);

  const CollisionBox* const box = components().get<CollisionBox>(enemy);
  ASSERT_NE(box, nullptr);
  EXPECT_EQ(box->width, ASKED_BOX.width);
  EXPECT_EQ(box->height, ASKED_BOX.height);
}

/**
 * Given an enemy with a top speed whose behaviour asks for another one
 * When the pilots run once
 * Then the enemy has the top speed it asked for
 */
TEST_F(PilotSystemFixture, ABodyTakesTheTopSpeedItsBehaviourAsks) {
  const ActionRequestingBehaviour behaviour{
      Intent{.steered = true, .topSpeedTo = ASKED_TOP_SPEED}};
  const Entity enemy = addEnemy(behaviour, WATCHING_ENEMY_NETWORK_ID);
  components().add(enemy, SteadyMotion{.topSpeed = STARTING_TOP_SPEED});

  run(SIMULATION_TICK_DURATION);

  const SteadyMotion* const motion = components().get<SteadyMotion>(enemy);
  ASSERT_NE(motion, nullptr);
  EXPECT_EQ(motion->topSpeed, ASKED_TOP_SPEED);
}
