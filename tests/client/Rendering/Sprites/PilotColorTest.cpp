#include <gtest/gtest.h>
#include "PilotColor.hpp"
#include "PilotTestConstants.hpp"

using rtype::client::pilotColorOfPlayerNumber;
using rtype::client::playerNumberOfPilotColor;

/**
 * Given player 3
 * When its pilot color is asked for
 * Then it is the third color, the third row of the sheets
 */
TEST(PilotColor, PlayerThreeFliesTheThirdColor) {
  EXPECT_EQ(pilotColorOfPlayerNumber(THIRD_PLAYER_NUMBER), THIRD_PILOT_COLOR);
}

/**
 * Given the third pilot color
 * When the number of the player who flies it is asked for
 * Then it is player 3
 */
TEST(PilotColor, TheThirdColorIsFlownByPlayerThree) {
  EXPECT_EQ(playerNumberOfPilotColor(THIRD_PILOT_COLOR), THIRD_PLAYER_NUMBER);
}
