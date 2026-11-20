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
#include "SpinCountdown.hpp"

using rtype::client::MotionPreference;
using rtype::client::RouletteReading;
using rtype::client::rouletteViewOf;
using rtype::client::SpinCountdown;
using rtype::client::testing::A_ROUND;
using rtype::client::testing::bettingRoom;
using rtype::client::testing::FIRST_IDLE_FRAME;
using rtype::client::testing::HELD_GREETING_FRAME;
using rtype::client::testing::INTO_THE_GREETING_OF_A_CYCLE;
using rtype::client::testing::INTO_THE_IDLE_OF_A_CYCLE;
using rtype::client::testing::SECOND_GREETING_FRAME;
using rtype::client::testing::SECOND_IDLE_FRAME;
using rtype::client::testing::spinningRoom;
using rtype::client::testing::TWELVE_OF_TWENTY_SECONDS;
using rtype::client::testing::TWELVE_SECONDS;
using rtype::client::testing::TWO_SECONDS;
using rtype::client::testing::TWO_SECONDS_BEFORE_THE_SPIN;
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

/**
 * Given a table whose bets just opened, twelve seconds before the spin
 * When it is read
 * Then the plaque under the croupier counts the twelve seconds, its bar 12
 * of its 20 seconds full, not yet red
 */
TEST(RouletteScene, ThePlaqueCountsTheSecondsBeforeTheSpin) {
  RouletteReading reading;
  reading.room = bettingRoom(A_ROUND);

  const SpinCountdown countdown =
      rouletteViewOf(reading).countdown.value_or(SpinCountdown{});

  ASSERT_TRUE(rouletteViewOf(reading).countdown.has_value());
  EXPECT_EQ(countdown.seconds, TWELVE_SECONDS);
  EXPECT_FLOAT_EQ(countdown.share, TWELVE_OF_TWENTY_SECONDS);
  EXPECT_FALSE(countdown.hurry);
}

/**
 * Given a table two seconds before its spin
 * When it is read
 * Then the plaque counts its last seconds in red
 */
TEST(RouletteScene, TheLastSecondsBeforeTheSpinHurry) {
  CasinoRoomState room = bettingRoom(A_ROUND);
  room.serverTime = room.closesAt - TWO_SECONDS_BEFORE_THE_SPIN;
  RouletteReading reading;
  reading.room = room;

  const SpinCountdown countdown =
      rouletteViewOf(reading).countdown.value_or(SpinCountdown{});

  EXPECT_EQ(countdown.seconds, TWO_SECONDS);
  EXPECT_TRUE(countdown.hurry);
}

/**
 * Given a table whose wheel spins
 * When it is read
 * Then no plaque counts down: the bets are closed
 */
TEST(RouletteScene, NoPlaqueOnceTheWheelSpins) {
  RouletteReading reading;
  reading.room = spinningRoom();

  EXPECT_FALSE(rouletteViewOf(reading).countdown.has_value());
}
