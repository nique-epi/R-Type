#include <gtest/gtest.h>
#include <cstddef>
#include "Position.hpp"
#include "PredictionConstants.hpp"
#include "PredictionFixtures.hpp"
#include "PredictionTestConstants.hpp"
#include "ShipCorrection.hpp"
#include "ShipCorrectionTestConstants.hpp"

using rtype::client::ShipCorrection;
using rtype::client::SNAP_DISTANCE;
using rtype::client::testing::ABOVE_THE_SNAP_DISTANCE;
using rtype::client::testing::distanceBetween;
using rtype::client::testing::ORIGIN;
using rtype::client::testing::POSITION_TOLERANCE;
using rtype::client::testing::PREDICTED;
using rtype::client::testing::SHOWN_FAR_BEHIND;
using rtype::client::testing::SHOWN_SIXTY_BEHIND;
using rtype::client::testing::SIXTY_UNITS_AFTER_ONE_TICK;
using rtype::client::testing::TICKS_TO_CLOSE_A_GAP;
using rtype::game::Position;

/**
 * Given a ship shown 60 units behind the ship predicted
 * When the correction absorbs the gap
 * Then the ship is still shown where it was, not on the prediction
 */
TEST(ShipCorrection, KeepsTheShipWhereItWasShownWhenTheGapIsSmall) {
  ShipCorrection correction;

  correction.absorb(SHOWN_SIXTY_BEHIND, PREDICTED);

  const Position shown = correction.shownFor(PREDICTED);
  EXPECT_FLOAT_EQ(shown.x, SHOWN_SIXTY_BEHIND.x);
  EXPECT_FLOAT_EQ(shown.y, SHOWN_SIXTY_BEHIND.y);
}

/**
 * Given a ship shown 460 units behind the ship predicted
 * When the correction absorbs the gap
 * Then the ship is shown on the prediction at once
 */
TEST(ShipCorrection, ShowsThePredictionAtOnceWhenTheGapIsLarge) {
  ShipCorrection correction;

  correction.absorb(SHOWN_FAR_BEHIND, PREDICTED);

  const Position shown = correction.shownFor(PREDICTED);
  EXPECT_FLOAT_EQ(shown.x, PREDICTED.x);
  EXPECT_FLOAT_EQ(shown.y, PREDICTED.y);
}

/**
 * Given a gap of exactly the snap distance, and one a unit larger
 * When each is absorbed
 * Then the first is eased and the second shown on the prediction
 */
TEST(ShipCorrection, EasesAGapOfTheSnapDistanceAndSnapsOneAboveIt) {
  ShipCorrection eased;
  ShipCorrection snapped;

  eased.absorb(Position{.x = SNAP_DISTANCE, .y = 0.0F}, ORIGIN);
  snapped.absorb(
      Position{.x = SNAP_DISTANCE + ABOVE_THE_SNAP_DISTANCE, .y = 0.0F},
      ORIGIN);

  EXPECT_FLOAT_EQ(eased.shownFor(ORIGIN).x, SNAP_DISTANCE);
  EXPECT_FLOAT_EQ(snapped.shownFor(ORIGIN).x, 0.0F);
}

/**
 * Given a gap of 60 units absorbed
 * When one tick passes
 * Then the ship is shown 54 units from the prediction: it kept 90 percent of
 * the gap and moved toward the prediction
 */
TEST(ShipCorrection, ClosesTheGapByAShareOfItEachTick) {
  ShipCorrection correction;
  correction.absorb(SHOWN_SIXTY_BEHIND, PREDICTED);

  correction.decay();

  const Position shown = correction.shownFor(PREDICTED);
  EXPECT_NEAR(distanceBetween(shown, PREDICTED), SIXTY_UNITS_AFTER_ONE_TICK,
              POSITION_TOLERANCE);
  EXPECT_LT(shown.x, PREDICTED.x);
  EXPECT_GT(shown.x, SHOWN_SIXTY_BEHIND.x);
}

/**
 * Given a gap of 60 units absorbed
 * When a hundred ticks pass
 * Then the ship is shown exactly on the prediction
 */
TEST(ShipCorrection, ShowsThePredictionOnceTheGapIsClosed) {
  ShipCorrection correction;
  correction.absorb(SHOWN_SIXTY_BEHIND, PREDICTED);

  for (std::size_t tick = 0; tick < TICKS_TO_CLOSE_A_GAP; ++tick) {
    correction.decay();
  }

  const Position shown = correction.shownFor(PREDICTED);
  EXPECT_FLOAT_EQ(shown.x, PREDICTED.x);
  EXPECT_FLOAT_EQ(shown.y, PREDICTED.y);
}

/**
 * Given a gap of 60 units absorbed
 * When the correction is cleared
 * Then the ship is shown on the prediction
 */
TEST(ShipCorrection, ShowsThePredictionAfterItIsCleared) {
  ShipCorrection correction;
  correction.absorb(SHOWN_SIXTY_BEHIND, PREDICTED);

  correction.clear();

  EXPECT_FLOAT_EQ(correction.shownFor(PREDICTED).x, PREDICTED.x);
}

/**
 * Given a correction that absorbed no gap
 * When the ship is shown
 * Then it is shown on the prediction
 */
TEST(ShipCorrection, ShowsThePredictionWhenNothingWasAbsorbed) {
  const ShipCorrection correction;

  EXPECT_FLOAT_EQ(correction.shownFor(PREDICTED).x, PREDICTED.x);
  EXPECT_FLOAT_EQ(correction.shownFor(PREDICTED).y, PREDICTED.y);
}
