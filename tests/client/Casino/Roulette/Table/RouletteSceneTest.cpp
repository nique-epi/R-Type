#include <gtest/gtest.h>
#include <chrono>
#include "CasinoRoomState.hpp"
#include "MillisecondDate.hpp"
#include "MotionPreference.hpp"
#include "RouletteReading.hpp"
#include "RouletteScene.hpp"
#include "RouletteSceneTestConstants.hpp"
#include "RouletteScreenFixtures.hpp"
#include "RouletteScreenTestConstants.hpp"
#include "RouletteView.hpp"

using rtype::client::MotionPreference;
using rtype::client::RouletteReading;
using rtype::client::rouletteViewOf;
using rtype::client::testing::A_ROUND;
using rtype::client::testing::bettingRoom;
using rtype::client::testing::FIRST_IDLE_FRAME;
using rtype::client::testing::HELD_GREETING_FRAME;
using rtype::client::testing::INTO_THE_GREETING_OF_A_CYCLE;
using rtype::client::testing::INTO_THE_IDLE_OF_A_CYCLE;
using rtype::client::testing::SECOND_GREETING_FRAME;
using rtype::client::testing::SECOND_IDLE_FRAME;
using rtype::client::testing::spinningRoom;
using rtype::network::CasinoRoomState;
using rtype::network::MillisecondDate;

/**
 * Given a table whose bets are open, early in a cycle of the croupier
 * When it is read
 * Then the croupier stands idle, on the idle frame of that time
 */
TEST(RouletteScene, TheCroupierStandsIdleBetweenHisGreetings) {
  CasinoRoomState room = bettingRoom(A_ROUND);
  room.serverTime = MillisecondDate{INTO_THE_IDLE_OF_A_CYCLE};
  RouletteReading reading;
  reading.room = room;

  EXPECT_EQ(rouletteViewOf(reading).croupier, SECOND_IDLE_FRAME);
}

/**
 * Given a table whose bets are open, late in a cycle of the croupier
 * When it is read
 * Then the croupier greets the players
 */
TEST(RouletteScene, TheCroupierGreetsWhileTheBetsAreOpen) {
  CasinoRoomState room = bettingRoom(A_ROUND);
  room.serverTime = MillisecondDate{INTO_THE_GREETING_OF_A_CYCLE};
  RouletteReading reading;
  reading.room = room;

  EXPECT_EQ(rouletteViewOf(reading).croupier, SECOND_GREETING_FRAME);
}

/**
 * Given a table whose wheel spins, at the time he greets while the bets are
 * open
 * When it is read
 * Then the croupier stands idle: he greets only for the bets
 */
TEST(RouletteScene, TheCroupierGreetsNobodyWhileTheWheelSpins) {
  CasinoRoomState room = spinningRoom();
  room.serverTime = MillisecondDate{INTO_THE_GREETING_OF_A_CYCLE};
  RouletteReading reading;
  reading.room = room;

  EXPECT_EQ(rouletteViewOf(reading).croupier, FIRST_IDLE_FRAME);
}

/**
 * Given a table whose bets are open, read with less motion
 * When it is read at any time
 * Then the croupier holds one frame of his greeting
 */
TEST(RouletteScene, LessMotionHoldsTheCroupierStill) {
  CasinoRoomState room = bettingRoom(A_ROUND);
  room.serverTime = MillisecondDate{INTO_THE_IDLE_OF_A_CYCLE};
  RouletteReading reading;
  reading.room = room;
  reading.motion = MotionPreference::Reduced;

  EXPECT_EQ(rouletteViewOf(reading).croupier, HELD_GREETING_FRAME);
}
