#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include "PredictionFixtures.hpp"
#include "PredictionTestConstants.hpp"
#include "ShipControl.hpp"
#include "ShipPrediction.hpp"
#include "TimeConstants.hpp"

using rtype::client::ShipPrediction;
using rtype::client::testing::A_GAP_TO_GLIDE_OVER;
using rtype::client::testing::BEYOND_THE_SNAP_DISTANCE;
using rtype::client::testing::BEYOND_THE_TOLERANCE;
using rtype::client::testing::COMMAND_AFTER_THE_DOUBLE_TAP_WINDOW;
using rtype::client::testing::FIRST_COMMAND;
using rtype::client::testing::HOLDING_DOWN;
using rtype::client::testing::HOLDING_NOTHING;
using rtype::client::testing::HOLDING_RIGHT;
using rtype::client::testing::INSIDE_THE_TOLERANCE;
using rtype::client::testing::OLDER_THAN_ANY_KEPT_COMMAND;
using rtype::client::testing::POSITION_TOLERANCE;
using rtype::client::testing::SECOND_COMMAND;
using rtype::client::testing::shiftedRight;
using rtype::client::testing::shipAfterCommands;
using rtype::client::testing::START;
using rtype::client::testing::THIRD_COMMAND;
using rtype::client::testing::TICKS_TO_CLOSE_A_GAP;
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
 * position far beyond the snap distance from the one predicted
 * When the confirmation is reconciled
 * Then the ship is shown at once where the replay puts it: from the server
 * position with the motion kept after the second command, the third command
 * replayed only
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
      shiftedRight(afterTwo.position, BEYOND_THE_SNAP_DISTANCE);

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

/**
 * Given three commands predicted and the server confirming the second 40
 * units from the one predicted, which is beyond the tolerance and within the
 * snap distance
 * When the confirmation is reconciled
 * Then the ship is still shown where it was, not where the replay puts it
 */
TEST(ShipPrediction, DoesNotJumpWhenTheGapIsBeyondTheToleranceButNotLarge) {
  ShipPrediction prediction;
  prediction.restart(START);
  prediction.predict(HOLDING_RIGHT, FIRST_COMMAND);
  prediction.predict(HOLDING_RIGHT, SECOND_COMMAND);
  prediction.predict(HOLDING_RIGHT, THIRD_COMMAND);
  const rtype::game::Position shownBefore = prediction.position();

  prediction.reconcile(
      shiftedRight(shipAfterCommands(HOLDING_RIGHT, TWO_COMMANDS).position,
                   A_GAP_TO_GLIDE_OVER),
      SECOND_COMMAND);

  EXPECT_FLOAT_EQ(prediction.position().x, shownBefore.x);
  EXPECT_FLOAT_EQ(prediction.position().y, shownBefore.y);
}

/**
 * Given a prediction corrected 40 units to the right and one that never was,
 * both moving right from the start
 * When both predict the same commands for a hundred more ticks
 * Then the corrected one ends exactly 40 units to the right of the other:
 * the gap closed and the replay is the position shown
 */
TEST(ShipPrediction, GlidesToTheReplayedShipOnceTheGapIsClosed) {
  ShipPrediction corrected;
  ShipPrediction untouched;
  corrected.restart(START);
  untouched.restart(START);
  for (std::uint32_t number = FIRST_COMMAND; number <= THIRD_COMMAND;
       ++number) {
    corrected.predict(HOLDING_RIGHT, number);
    untouched.predict(HOLDING_RIGHT, number);
  }
  corrected.reconcile(
      shiftedRight(shipAfterCommands(HOLDING_RIGHT, TWO_COMMANDS).position,
                   A_GAP_TO_GLIDE_OVER),
      SECOND_COMMAND);

  std::uint32_t number = THIRD_COMMAND;
  for (std::size_t tick = 0; tick < TICKS_TO_CLOSE_A_GAP; ++tick) {
    corrected.predict(HOLDING_RIGHT, ++number);
    untouched.predict(HOLDING_RIGHT, number);
  }

  EXPECT_NEAR(corrected.position().x - untouched.position().x,
              A_GAP_TO_GLIDE_OVER, POSITION_TOLERANCE);
  EXPECT_NEAR(corrected.position().y, untouched.position().y,
              POSITION_TOLERANCE);
}

/**
 * Given a correction of 40 units that did not close yet
 * When the server corrects the ship again, 40 units from the replay
 * Then the ship is still shown where it was: the second correction starts
 * from the position shown, not from the replay
 */
TEST(ShipPrediction, StartsASecondCorrectionFromTheShipShown) {
  ShipPrediction prediction;
  prediction.restart(START);
  prediction.predict(HOLDING_RIGHT, FIRST_COMMAND);
  prediction.predict(HOLDING_RIGHT, SECOND_COMMAND);
  prediction.predict(HOLDING_RIGHT, THIRD_COMMAND);
  const rtype::game::MovingShip afterTwo =
      shipAfterCommands(HOLDING_RIGHT, TWO_COMMANDS);
  const rtype::game::Position firstServer =
      shiftedRight(afterTwo.position, A_GAP_TO_GLIDE_OVER);
  prediction.reconcile(firstServer, SECOND_COMMAND);
  const rtype::game::MovingShip replayedAfterThree = rtype::game::shipAfter(
      rtype::game::MovingShip{.position = firstServer,
                              .motion = afterTwo.motion},
      HOLDING_RIGHT, rtype::engine::SIMULATION_TICK_DURATION);
  const rtype::game::Position shownBefore = prediction.position();

  prediction.reconcile(
      shiftedRight(replayedAfterThree.position, A_GAP_TO_GLIDE_OVER),
      THIRD_COMMAND);

  EXPECT_FLOAT_EQ(prediction.position().x, shownBefore.x);
  EXPECT_FLOAT_EQ(prediction.position().y, shownBefore.y);
}

/**
 * Given a correction of 40 units that did not close yet
 * When the prediction restarts from the start
 * Then the ship is shown at the start
 */
TEST(ShipPrediction, ForgetsTheCorrectionWhenItRestarts) {
  ShipPrediction prediction;
  prediction.restart(START);
  prediction.predict(HOLDING_RIGHT, FIRST_COMMAND);
  prediction.predict(HOLDING_RIGHT, SECOND_COMMAND);
  prediction.reconcile(
      shiftedRight(shipAfterCommands(HOLDING_RIGHT, TWO_COMMANDS).position,
                   A_GAP_TO_GLIDE_OVER),
      SECOND_COMMAND);

  prediction.restart(START);

  EXPECT_FLOAT_EQ(prediction.position().x, START.x);
  EXPECT_FLOAT_EQ(prediction.position().y, START.y);
}
