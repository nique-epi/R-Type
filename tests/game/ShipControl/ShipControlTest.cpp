#include <gtest/gtest.h>
#include <cmath>
#include "ManeuverKind.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ShipControl.hpp"
#include "ShipControlConstants.hpp"
#include "ShipControlFixtures.hpp"
#include "ShipControlTestConstants.hpp"
#include "ShipMotion.hpp"
#include "TimeConstants.hpp"

using rtype::game::HeldChange;
using rtype::game::ManeuverKind;
using rtype::game::motionAfterInput;
using rtype::game::MovingShip;
using rtype::game::PLAY_AREA_HEIGHT;
using rtype::game::PLAYFIELD_WIDTH;
using rtype::game::SHIP_SPEED;
using rtype::game::ShipDirections;
using rtype::game::ShipMotion;
using rtype::game::testing::BACKSTEP_BACK_DISTANCE;
using rtype::game::testing::BOTTOM_EDGE_STOP_Y;
using rtype::game::testing::COOLDOWN_TICKS;
using rtype::game::testing::DASH_END_TICK;
using rtype::game::testing::DASH_FORWARD_DISTANCE;
using rtype::game::testing::EdgeApproach;
using rtype::game::testing::FIRST_TICK_DISTANCE;
using rtype::game::testing::FULL_SPEED_TICK;
using rtype::game::testing::GLIDE_DISTANCE;
using rtype::game::testing::HELD_SECOND_DISTANCE;
using rtype::game::testing::LATE_SECOND_PRESS;
using rtype::game::testing::LEFT_EDGE_STOP_X;
using rtype::game::testing::motionAfterTwoPresses;
using rtype::game::testing::NEAR_AN_EDGE;
using rtype::game::testing::OppositeDirections;
using rtype::game::testing::PAST_FULL_SPEED_TICKS;
using rtype::game::testing::PLAY_AREA_CENTER;
using rtype::game::testing::POSITION_TOLERANCE;
using rtype::game::testing::PRESS_AFTER_THE_DASH;
using rtype::game::testing::QUICK_SECOND_PRESS;
using rtype::game::testing::RIGHT_EDGE_STOP_X;
using rtype::game::testing::ROLL_DISTANCE;
using rtype::game::testing::ROLL_END_TICK;
using rtype::game::testing::SHIP_AT_REST;
using rtype::game::testing::shipAfterTicks;
using rtype::game::testing::ShipControlEdgeTest;
using rtype::game::testing::ShipControlOppositesTest;
using rtype::game::testing::ShipControlTapTest;
using rtype::game::testing::STOP_TICK;
using rtype::game::testing::TappedDirection;
using rtype::game::testing::TOP_EDGE_STOP_Y;

/**
 * Given a ship at rest in the middle of the play area whose player holds right
 * When one tick less than FULL_SPEED_TICK passes
 * Then the ship goes slower than SHIP_SPEED: it is still gathering speed
 */
TEST(ShipControl, IsBelowTheShipSpeedTheTickBeforeFullSpeed) {
  const MovingShip ship = shipAfterTicks(
      SHIP_AT_REST, ShipDirections{.right = true}, FULL_SPEED_TICK - 1);

  EXPECT_LT(ship.motion.velocityX, SHIP_SPEED);
}

/**
 * Given a ship at rest in the middle of the play area whose player holds right
 * When FULL_SPEED_TICK ticks pass
 * Then the ship goes right at exactly SHIP_SPEED
 */
TEST(ShipControl, ReachesTheShipSpeedOnTheFullSpeedTick) {
  const MovingShip ship = shipAfterTicks(
      SHIP_AT_REST, ShipDirections{.right = true}, FULL_SPEED_TICK);

  EXPECT_FLOAT_EQ(ship.motion.velocityX, SHIP_SPEED);
}

/**
 * Given a ship at rest in the middle of the play area whose player holds right
 * When its first tick passes
 * Then it has drifted FIRST_TICK_DISTANCE: a start, not a jump to full speed
 */
TEST(ShipControl, DriftsTheFirstTickDistanceOnItsFirstTick) {
  const MovingShip ship =
      shipAfterTicks(SHIP_AT_REST, ShipDirections{.right = true}, 1);

  EXPECT_NEAR(ship.position.x, PLAY_AREA_CENTER.x + FIRST_TICK_DISTANCE,
              POSITION_TOLERANCE);
}

/**
 * Given a ship at rest in the middle of the play area whose player holds right
 * When SIMULATION_TICKS_PER_SECOND ticks pass
 * Then it has drifted HELD_SECOND_DISTANCE to the right
 */
TEST(ShipControl, DriftsTheHeldSecondDistanceInASecond) {
  const MovingShip ship =
      shipAfterTicks(SHIP_AT_REST, ShipDirections{.right = true},
                     rtype::engine::SIMULATION_TICKS_PER_SECOND);

  EXPECT_NEAR(ship.position.x, PLAY_AREA_CENTER.x + HELD_SECOND_DISTANCE,
              POSITION_TOLERANCE);
}

/**
 * Given a ship going right at SHIP_SPEED whose player lets go
 * When one tick less than STOP_TICK passes
 * Then the ship still glides
 */
TEST(ShipControl, StillGlidesTheTickBeforeItStops) {
  const MovingShip ship =
      shipAfterTicks(MovingShip{.position = PLAY_AREA_CENTER,
                                .motion = {.velocityX = SHIP_SPEED}},
                     ShipDirections{}, STOP_TICK - 1);

  EXPECT_GT(ship.motion.velocityX, 0.0F);
}

/**
 * Given a ship going right at SHIP_SPEED whose player lets go
 * When STOP_TICK ticks pass
 * Then the ship has stopped
 */
TEST(ShipControl, StopsOnTheStopTick) {
  const MovingShip ship =
      shipAfterTicks(MovingShip{.position = PLAY_AREA_CENTER,
                                .motion = {.velocityX = SHIP_SPEED}},
                     ShipDirections{}, STOP_TICK);

  EXPECT_FLOAT_EQ(ship.motion.velocityX, 0.0F);
}

/**
 * Given a ship going right at SHIP_SPEED whose player lets go
 * When it has stopped
 * Then it has glided GLIDE_DISTANCE to the right, on the same row
 */
TEST(ShipControl, GlidesTheGlideDistanceOnceLetGo) {
  const MovingShip ship =
      shipAfterTicks(MovingShip{.position = PLAY_AREA_CENTER,
                                .motion = {.velocityX = SHIP_SPEED}},
                     ShipDirections{}, STOP_TICK);

  EXPECT_NEAR(ship.position.x, PLAY_AREA_CENTER.x + GLIDE_DISTANCE,
              POSITION_TOLERANCE);
  EXPECT_FLOAT_EQ(ship.position.y, PLAY_AREA_CENTER.y);
}

/**
 * Given a ship at rest in the middle of the play area whose player holds
 * nothing
 * When PAST_FULL_SPEED_TICKS ticks pass
 * Then the ship is where it was
 */
TEST(ShipControl, StaysStillWhenNothingIsHeld) {
  const MovingShip ship =
      shipAfterTicks(SHIP_AT_REST, ShipDirections{}, PAST_FULL_SPEED_TICKS);

  EXPECT_FLOAT_EQ(ship.position.x, PLAY_AREA_CENTER.x);
  EXPECT_FLOAT_EQ(ship.position.y, PLAY_AREA_CENTER.y);
}

/**
 * Given a ship at rest in the middle of the play area whose player holds two
 * opposite directions
 * When PAST_FULL_SPEED_TICKS ticks pass
 * Then the ship is where it was: the two cancel out
 */
TEST_P(ShipControlOppositesTest, StaysStillWhenOppositeDirectionsAreHeld) {
  const MovingShip ship =
      shipAfterTicks(SHIP_AT_REST, GetParam().held, PAST_FULL_SPEED_TICKS);

  EXPECT_FLOAT_EQ(ship.position.x, PLAY_AREA_CENTER.x);
  EXPECT_FLOAT_EQ(ship.position.y, PLAY_AREA_CENTER.y);
}

INSTANTIATE_TEST_SUITE_P(
    EachAxis, ShipControlOppositesTest,
    ::testing::Values(OppositeDirections{.name = "UpAndDown",
                                         .held = {.up = true, .down = true}},
                      OppositeDirections{
                          .name = "LeftAndRight",
                          .held = {.left = true, .right = true}}));

/**
 * Given a ship at rest whose player holds up, down and right
 * When FULL_SPEED_TICK ticks pass
 * Then the ship goes right at SHIP_SPEED and not along the other axis: up and
 * down leave a straight line, at full speed
 */
TEST(ShipControl, ReachesFullSpeedWhenOppositesLeaveAStraightLine) {
  const MovingShip ship = shipAfterTicks(
      SHIP_AT_REST, ShipDirections{.up = true, .down = true, .right = true},
      FULL_SPEED_TICK);

  EXPECT_FLOAT_EQ(ship.motion.velocityX, SHIP_SPEED);
  EXPECT_FLOAT_EQ(ship.motion.velocityY, 0.0F);
}

/**
 * Given a ship at rest whose player holds up and right
 * When PAST_FULL_SPEED_TICKS ticks pass
 * Then the ship goes at SHIP_SPEED, no more, as fast to the right as upwards
 */
TEST(ShipControl, IsNoFasterAlongADiagonalThanAlongAStraightLine) {
  const MovingShip ship =
      shipAfterTicks(SHIP_AT_REST, ShipDirections{.up = true, .right = true},
                     PAST_FULL_SPEED_TICKS);

  EXPECT_NEAR(std::hypot(ship.motion.velocityX, ship.motion.velocityY),
              SHIP_SPEED, POSITION_TOLERANCE);
  EXPECT_NEAR(ship.motion.velocityX, -ship.motion.velocityY,
              POSITION_TOLERANCE);
}

/**
 * Given a ship at rest NEAR_AN_EDGE from an edge whose player holds the
 * direction of that edge
 * When PAST_FULL_SPEED_TICKS ticks pass, long enough to take its center past
 * the edge
 * Then the ship stops with its side on the edge: the whole ship stays in sight
 */
TEST_P(ShipControlEdgeTest, StopsWithItsSideOnTheEdge) {
  const EdgeApproach approach = GetParam();

  const MovingShip ship =
      shipAfterTicks(MovingShip{.position = approach.from, .motion = {}},
                     approach.held, PAST_FULL_SPEED_TICKS);

  EXPECT_FLOAT_EQ(ship.position.x, approach.stop.x);
  EXPECT_FLOAT_EQ(ship.position.y, approach.stop.y);
}

INSTANTIATE_TEST_SUITE_P(
    EachEdge, ShipControlEdgeTest,
    ::testing::Values(
        EdgeApproach{.name = "RightEdge",
                     .held = {.right = true},
                     .from = {.x = PLAYFIELD_WIDTH - NEAR_AN_EDGE,
                              .y = PLAY_AREA_CENTER.y},
                     .stop = {.x = RIGHT_EDGE_STOP_X, .y = PLAY_AREA_CENTER.y}},
        EdgeApproach{.name = "LeftEdge",
                     .held = {.left = true},
                     .from = {.x = NEAR_AN_EDGE, .y = PLAY_AREA_CENTER.y},
                     .stop = {.x = LEFT_EDGE_STOP_X, .y = PLAY_AREA_CENTER.y}},
        EdgeApproach{.name = "TopEdge",
                     .held = {.up = true},
                     .from = {.x = PLAY_AREA_CENTER.x, .y = NEAR_AN_EDGE},
                     .stop = {.x = PLAY_AREA_CENTER.x, .y = TOP_EDGE_STOP_Y}},
        EdgeApproach{
            .name = "BottomEdge",
            .held = {.down = true},
            .from = {.x = PLAY_AREA_CENTER.x,
                     .y = PLAY_AREA_HEIGHT - NEAR_AN_EDGE},
            .stop = {.x = PLAY_AREA_CENTER.x, .y = BOTTOM_EDGE_STOP_Y}}));

/**
 * Given a ship whose side is on the right edge, going right at SHIP_SPEED
 * When the edge stops it for a tick
 * Then it has lost its speed to the right: it does not push on once let go
 */
TEST(ShipControl, LosesItsSpeedAgainstTheEdge) {
  const MovingShip ship = shipAfterTicks(
      MovingShip{.position = {.x = RIGHT_EDGE_STOP_X, .y = PLAY_AREA_CENTER.y},
                 .motion = {.velocityX = SHIP_SPEED}},
      ShipDirections{.right = true}, 1);

  EXPECT_FLOAT_EQ(ship.position.x, RIGHT_EDGE_STOP_X);
  EXPECT_FLOAT_EQ(ship.motion.velocityX, 0.0F);
}

/**
 * Given a ship whose center is past the right edge and above the top edge, and
 * whose player holds nothing
 * When a tick passes
 * Then the ship is brought back with its sides on those two edges
 */
TEST(ShipControl, BringsBackAShipThatWasNotInsideThePlayfield) {
  const MovingShip ship = shipAfterTicks(
      MovingShip{
          .position = {.x = PLAYFIELD_WIDTH + NEAR_AN_EDGE, .y = -NEAR_AN_EDGE},
          .motion = {}},
      ShipDirections{}, 1);

  EXPECT_FLOAT_EQ(ship.position.x, RIGHT_EDGE_STOP_X);
  EXPECT_FLOAT_EQ(ship.position.y, TOP_EDGE_STOP_Y);
}

/**
 * Given a ship at rest
 * When its player presses a direction, lets go, and presses it again within
 * the window
 * Then the maneuver of that direction starts, its whole distance set
 */
TEST_P(ShipControlTapTest, StartsTheManeuverOfASecondPressWithinTheWindow) {
  const TappedDirection tapped = GetParam();

  const ShipMotion motion =
      motionAfterTwoPresses(ShipMotion{}, tapped.held, QUICK_SECOND_PRESS);

  EXPECT_EQ(motion.maneuver, tapped.kind);
  EXPECT_FLOAT_EQ(motion.maneuverDistance.x, tapped.distance.x);
  EXPECT_FLOAT_EQ(motion.maneuverDistance.y, tapped.distance.y);
}

INSTANTIATE_TEST_SUITE_P(
    EachDirection, ShipControlTapTest,
    ::testing::Values(
        TappedDirection{.name = "DownRollsDown",
                        .held = {.down = true},
                        .kind = ManeuverKind::Roll,
                        .distance = {.x = 0.0F, .y = ROLL_DISTANCE}},
        TappedDirection{.name = "UpRollsUp",
                        .held = {.up = true},
                        .kind = ManeuverKind::Roll,
                        .distance = {.x = 0.0F, .y = -ROLL_DISTANCE}},
        TappedDirection{.name = "RightDashes",
                        .held = {.right = true},
                        .kind = ManeuverKind::Dash,
                        .distance = {.x = DASH_FORWARD_DISTANCE, .y = 0.0F}},
        TappedDirection{
            .name = "LeftBacksteps",
            .held = {.left = true},
            .kind = ManeuverKind::Dash,
            .distance = {.x = -BACKSTEP_BACK_DISTANCE, .y = 0.0F}}));

/**
 * Given a ship at rest
 * When its player presses down, lets go, and presses down again past the
 * window
 * Then no maneuver starts: two slow taps stay two small moves
 */
TEST(ShipControl, StartsNothingOnASecondPressPastTheWindow) {
  const ShipMotion motion = motionAfterTwoPresses(
      ShipMotion{}, ShipDirections{.down = true}, LATE_SECOND_PRESS);

  EXPECT_EQ(motion.maneuver, ManeuverKind::None);
}

/**
 * Given a ship at rest that starts a roll down, its player holding nothing
 * When one tick less than ROLL_END_TICK passes
 * Then the roll has not carried it its whole distance yet
 */
TEST(ShipControl, IsShortOfTheRollDistanceTheTickBeforeTheRollEnds) {
  const MovingShip ship = shipAfterTicks(
      MovingShip{
          .position = PLAY_AREA_CENTER,
          .motion = motionAfterTwoPresses(
              ShipMotion{}, ShipDirections{.down = true}, QUICK_SECOND_PRESS)},
      ShipDirections{}, ROLL_END_TICK - 1);

  EXPECT_LT(ship.position.y, PLAY_AREA_CENTER.y + ROLL_DISTANCE);
}

/**
 * Given a ship at rest that starts a roll down, its player holding nothing
 * When ROLL_END_TICK ticks pass
 * Then the ship is ROLL_DISTANCE lower, on the same column, and the roll is
 * over
 */
TEST(ShipControl, IsCarriedTheRollDistanceByTheEndOfTheRoll) {
  const MovingShip ship = shipAfterTicks(
      MovingShip{
          .position = PLAY_AREA_CENTER,
          .motion = motionAfterTwoPresses(
              ShipMotion{}, ShipDirections{.down = true}, QUICK_SECOND_PRESS)},
      ShipDirections{}, ROLL_END_TICK);

  EXPECT_NEAR(ship.position.y, PLAY_AREA_CENTER.y + ROLL_DISTANCE,
              POSITION_TOLERANCE);
  EXPECT_FLOAT_EQ(ship.position.x, PLAY_AREA_CENTER.x);
  EXPECT_EQ(ship.motion.maneuver, ManeuverKind::None);
}

/**
 * Given a ship whose dash, started by presses of right at FIRST_PRESS and
 * QUICK_SECOND_PRESS, ran to its end
 * When its player presses right again within the window of the second press
 * Then no dash starts: the press that started the dash is not the first press
 * of the next one
 */
TEST(ShipControl, StartsNoManeuverFromThePressThatStartedTheLast) {
  const MovingShip dashed = shipAfterTicks(
      MovingShip{
          .position = PLAY_AREA_CENTER,
          .motion = motionAfterTwoPresses(
              ShipMotion{}, ShipDirections{.right = true}, QUICK_SECOND_PRESS)},
      ShipDirections{}, DASH_END_TICK);

  ShipMotion motion = motionAfterInput(
      dashed.motion, HeldChange{.before = {.right = true},
                                .now = {},
                                .inputNumber = QUICK_SECOND_PRESS + 1});
  motion =
      motionAfterInput(motion, HeldChange{.before = {},
                                          .now = {.right = true},
                                          .inputNumber = PRESS_AFTER_THE_DASH});

  EXPECT_EQ(motion.maneuver, ManeuverKind::None);
}

/**
 * Given a ship whose roll ended one tick less than COOLDOWN_TICKS ago
 * When its player double presses up
 * Then no roll starts: the next roll waits out the cooldown
 */
TEST(ShipControl, StartsNoRollDuringTheCooldown) {
  const MovingShip rolled = shipAfterTicks(
      MovingShip{
          .position = PLAY_AREA_CENTER,
          .motion = motionAfterTwoPresses(
              ShipMotion{}, ShipDirections{.down = true}, QUICK_SECOND_PRESS)},
      ShipDirections{}, ROLL_END_TICK + COOLDOWN_TICKS - 1);

  const ShipMotion motion = motionAfterTwoPresses(
      rolled.motion, ShipDirections{.up = true}, QUICK_SECOND_PRESS);

  EXPECT_EQ(motion.maneuver, ManeuverKind::None);
}
