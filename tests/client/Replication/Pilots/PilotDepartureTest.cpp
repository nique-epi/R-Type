#include <gtest/gtest.h>
#include "PilotDeparture.hpp"
#include "PilotStatus.hpp"

using rtype::client::hasLeft;
using rtype::client::PilotState;
using rtype::client::PilotStatus;

/**
 * Given a pilot that left, one whose signal is lost and one the host expelled
 * When each is asked whether it left the game
 * Then each has
 */
TEST(PilotDeparture, APilotGoneByEachDepartureHasLeft) {
  for (const PilotState state :
       {PilotState::Left, PilotState::SignalLost, PilotState::Expelled}) {
    PilotStatus pilot{};
    pilot.state = state;

    EXPECT_TRUE(hasLeft(pilot)) << static_cast<int>(state);
  }
}

/**
 * Given a pilot flying, one respawning and one out of lives
 * When each is asked whether it left the game
 * Then none has: a pilot out of lives still watches the game
 */
TEST(PilotDeparture, APilotStillInTheGameHasNotLeft) {
  for (const PilotState state :
       {PilotState::Flying, PilotState::Respawning, PilotState::OutOfLives}) {
    PilotStatus pilot{};
    pilot.state = state;

    EXPECT_FALSE(hasLeft(pilot)) << static_cast<int>(state);
  }
}
