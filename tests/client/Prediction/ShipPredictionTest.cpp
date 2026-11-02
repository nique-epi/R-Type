#include <gtest/gtest.h>
#include "PredictionFixtures.hpp"
#include "PredictionTestConstants.hpp"
#include "ShipControl.hpp"
#include "ShipPrediction.hpp"
#include "TimeConstants.hpp"

using rtype::client::ShipPrediction;
using rtype::client::testing::BEYOND_THE_TOLERANCE;
using rtype::client::testing::COMMAND_AFTER_THE_DOUBLE_TAP_WINDOW;
using rtype::client::testing::FIRST_COMMAND;
using rtype::client::testing::HOLDING_DOWN;
using rtype::client::testing::HOLDING_NOTHING;
using rtype::client::testing::HOLDING_RIGHT;
using rtype::client::testing::INSIDE_THE_TOLERANCE;
using rtype::client::testing::OLDER_THAN_ANY_KEPT_COMMAND;
using rtype::client::testing::SECOND_COMMAND;
using rtype::client::testing::shiftedRight;
using rtype::client::testing::shipAfterCommands;
using rtype::client::testing::START;
using rtype::client::testing::THIRD_COMMAND;
using rtype::client::testing::THREE_COMMANDS;
using rtype::client::testing::TWO_COMMANDS;

/**
 * Given a prediction started at rest
 * When it predicts a command holding right
 * Then the ship is where the server puts a ship after one such tick
 */
TEST(ShipPrediction, MovesTheShipAtOnceWhenAKeyIsHeld) {
  ShipPrediction prediction;
  prediction.restart(START);

  prediction.predict(HOLDING_RIGHT, FIRST_COMMAND);

  const rtype::game::MovingShip server =
      shipAfterCommands(HOLDING_RIGHT, FIRST_COMMAND);
  EXPECT_GT(prediction.position().x, START.x);
  EXPECT_FLOAT_EQ(prediction.position().x, server.position.x);
  EXPECT_FLOAT_EQ(prediction.position().y, server.position.y);
}

/**
 * Given a prediction that was never started
 * When it predicts a command
 * Then it stays stopped and the position does not move
 */
TEST(ShipPrediction, PredictsNothingBeforeItStarts) {
  ShipPrediction prediction;

  prediction.predict(HOLDING_RIGHT, FIRST_COMMAND);

  EXPECT_FALSE(prediction.isStarted());
  EXPECT_FLOAT_EQ(prediction.position().x, 0.0F);
}

/**
 * Given three commands predicted and the server confirming the second at a
 * position half the tolerance from the one predicted
 * When the confirmation is reconciled
 * Then the ship stays where the client predicted it
 */
TEST(ShipPrediction, KeepsItsPositionWhenTheServerIsWithinTheTolerance) {
  ShipPrediction prediction;
  prediction.restart(START);
  prediction.predict(HOLDING_RIGHT, FIRST_COMMAND);
  prediction.predict(HOLDING_RIGHT, SECOND_COMMAND);
  prediction.predict(HOLDING_RIGHT, THIRD_COMMAND);
  const rtype::game::Position predicted = prediction.position();

  prediction.reconcile(
      shiftedRight(shipAfterCommands(HOLDING_RIGHT, TWO_COMMANDS).position,
                   INSIDE_THE_TOLERANCE),
      SECOND_COMMAND);

  EXPECT_FLOAT_EQ(prediction.position().x, predicted.x);
  EXPECT_FLOAT_EQ(prediction.position().y, predicted.y);
}

/**
 * Given three commands predicted and the server confirming the second at a
 * position twice the tolerance from the one predicted
 * When the confirmation is reconciled
 * Then the ship restarts from the server position with the motion kept after
 * the second command, and replays the third command only
 */
TEST(ShipPrediction, TakesTheServerBackAndReplaysTheCommandsAfterTheConfirmed) {
  ShipPrediction prediction;
  prediction.restart(START);
  prediction.predict(HOLDING_RIGHT, FIRST_COMMAND);
  prediction.predict(HOLDING_RIGHT, SECOND_COMMAND);
  prediction.predict(HOLDING_RIGHT, THIRD_COMMAND);
  const rtype::game::MovingShip afterTwo =
      shipAfterCommands(HOLDING_RIGHT, TWO_COMMANDS);
  const rtype::game::Position server =
      shiftedRight(afterTwo.position, BEYOND_THE_TOLERANCE);

  prediction.reconcile(server, SECOND_COMMAND);

  const rtype::game::MovingShip expected = rtype::game::shipAfter(
      rtype::game::MovingShip{.position = server, .motion = afterTwo.motion},
      HOLDING_RIGHT, rtype::engine::SIMULATION_TICK_DURATION);
  EXPECT_FLOAT_EQ(prediction.position().x, expected.position.x);
  EXPECT_FLOAT_EQ(prediction.position().y, expected.position.y);
}

/**
 * Given three commands, the second confirmed far from the prediction, then a
 * confirmation of the first, which the client no longer keeps
 * When the older confirmation is reconciled
 * Then the ship does not move
 */
TEST(ShipPrediction, IgnoresAConfirmationOlderThanTheCommandsItKept) {
  ShipPrediction prediction;
  prediction.restart(START);
  prediction.predict(HOLDING_RIGHT, FIRST_COMMAND);
  prediction.predict(HOLDING_RIGHT, SECOND_COMMAND);
  prediction.predict(HOLDING_RIGHT, THIRD_COMMAND);
  prediction.reconcile(
      shiftedRight(shipAfterCommands(HOLDING_RIGHT, TWO_COMMANDS).position,
                   BEYOND_THE_TOLERANCE),
      SECOND_COMMAND);
  const rtype::game::Position settled = prediction.position();

  prediction.reconcile(shiftedRight(START, BEYOND_THE_TOLERANCE),
                       FIRST_COMMAND);

  EXPECT_FLOAT_EQ(prediction.position().x, settled.x);
  EXPECT_FLOAT_EQ(prediction.position().y, settled.y);
}

/**
 * Given a command predicted
 * When the server confirms 0, which it sends while it received no input
 * Then the ship does not move
 */
TEST(ShipPrediction, IgnoresAnAcknowledgementOfZero) {
  ShipPrediction prediction;
  prediction.restart(START);
  prediction.predict(HOLDING_RIGHT, FIRST_COMMAND);
  const rtype::game::Position predicted = prediction.position();

  prediction.reconcile(shiftedRight(START, BEYOND_THE_TOLERANCE),
                       OLDER_THAN_ANY_KEPT_COMMAND);

  EXPECT_FLOAT_EQ(prediction.position().x, predicted.x);
}

/**
 * Given a prediction started with no command predicted
 * When the server confirms a command
 * Then the ship stays at the start, there being nothing to compare
 */
TEST(ShipPrediction, ReconcilesNothingWhenNoCommandIsPending) {
  ShipPrediction prediction;
  prediction.restart(START);

  prediction.reconcile(shiftedRight(START, BEYOND_THE_TOLERANCE),
                       FIRST_COMMAND);

  EXPECT_FLOAT_EQ(prediction.position().x, START.x);
}

/**
 * Given a command predicted, then the prediction restarted from the start
 * When the server confirms that command
 * Then the ship stays at the start: the restart forgot the command
 */
TEST(ShipPrediction, ForgetsTheCommandsWhenItRestarts) {
  ShipPrediction prediction;
  prediction.restart(START);
  prediction.predict(HOLDING_RIGHT, FIRST_COMMAND);
  prediction.restart(START);

  prediction.reconcile(shiftedRight(START, BEYOND_THE_TOLERANCE),
                       FIRST_COMMAND);

  EXPECT_FLOAT_EQ(prediction.position().x, START.x);
}

/**
 * Given two predictions that press down, release, and press down again, one
 * within the double tap window and one past it
 * When both are predicted
 * Then the first rolled further down than the second, as the server rolls it
 */
TEST(ShipPrediction, RollsOnADoubleTapLikeTheServer) {
  ShipPrediction rolled;
  rolled.restart(START);
  rolled.predict(HOLDING_DOWN, FIRST_COMMAND);
  rolled.predict(HOLDING_NOTHING, SECOND_COMMAND);
  rolled.predict(HOLDING_DOWN, THIRD_COMMAND);
  ShipPrediction slow;
  slow.restart(START);
  slow.predict(HOLDING_DOWN, FIRST_COMMAND);
  slow.predict(HOLDING_NOTHING, SECOND_COMMAND);
  slow.predict(HOLDING_DOWN, COMMAND_AFTER_THE_DOUBLE_TAP_WINDOW);

  EXPECT_GT(rolled.position().y, slow.position().y);
}
