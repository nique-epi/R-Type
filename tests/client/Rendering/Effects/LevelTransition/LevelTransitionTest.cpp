#include <gtest/gtest.h>
#include "EffectTestConstants.hpp"
#include "LevelTransition.hpp"
#include "LevelTransitionTestConstants.hpp"

using rtype::client::shipShiftAt;
using rtype::client::transitionWarpAt;

/**
 * Given a transition longer than its three parts
 * When the ships are read before they fly out and as they start
 * Then they are drawn at their place
 */
TEST(LevelTransition, TheShipsStayInPlaceUntilTheyFlyOut) {
  EXPECT_FLOAT_EQ(shipShiftAt(BEFORE_THE_SHIPS_FLY_OUT), 0.0F);
  EXPECT_FLOAT_EQ(shipShiftAt(AS_THE_SHIPS_FLY_OUT), 0.0F);
}

/**
 * Given a transition whose ships flew out
 * When they are read while they are away
 * Then a ship at column 0 is drawn wholly past the right edge
 */
TEST(LevelTransition, TheShipsAreAwayPastTheRightEdge) {
  EXPECT_GT(shipShiftAt(WHILE_THE_SHIPS_ARE_AWAY), BEYOND_THE_RIGHT_EDGE);
}

/**
 * Given a transition whose ships were put back at their start
 * When they are read as they start flying back
 * Then a ship at its start is drawn wholly beyond the left edge
 */
TEST(LevelTransition, TheShipsFlyBackFromBeyondTheLeftEdge) {
  EXPECT_LT(COLUMN_OF_A_SHIP_START + shipShiftAt(AS_THE_SHIPS_FLY_BACK),
            BEYOND_THE_LEFT_EDGE);
}

/**
 * Given a transition about to end, then one that ended
 * When the ships are read
 * Then they are drawn at their place
 */
TEST(LevelTransition, TheShipsAreBackInPlaceAtTheEnd) {
  EXPECT_NEAR(shipShiftAt(A_MILLISECOND_LEFT), 0.0F, SHIFT_TOLERANCE);
  EXPECT_FLOAT_EQ(shipShiftAt(NO_TIME_LEFT), 0.0F);
}

/**
 * Given a transition whose ships are away
 * When the stars are read
 * Then they go 12 times their speed, the top of the warp
 */
TEST(LevelTransition, TheStarsWarpWhileTheShipsAreAway) {
  EXPECT_NEAR(transitionWarpAt(WHILE_THE_SHIPS_ARE_AWAY), TOP_WARP_SPEED,
              WARP_FACTOR_TOLERANCE);
}

/**
 * Given no transition, then a transition before its ships fly out
 * When the stars are read
 * Then they go their own speed
 */
TEST(LevelTransition, TheStarsGoTheirOwnSpeedOutsideTheFlights) {
  EXPECT_NEAR(transitionWarpAt(NO_TIME_LEFT), OWN_SPEED, WARP_FACTOR_TOLERANCE);
  EXPECT_NEAR(transitionWarpAt(BEFORE_THE_SHIPS_FLY_OUT), OWN_SPEED,
              WARP_FACTOR_TOLERANCE);
}
