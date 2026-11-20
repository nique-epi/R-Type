#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <string>
#include <vector>
#include "BigWin.hpp"
#include "CasinoRoomState.hpp"
#include "InterfaceColorConstants.hpp"
#include "MillisecondDate.hpp"
#include "MotionPreference.hpp"
#include "OtherPlayerBets.hpp"
#include "RouletteReading.hpp"
#include "RouletteScene.hpp"
#include "RouletteSceneTestConstants.hpp"
#include "RouletteScreenFixtures.hpp"
#include "RouletteScreenTestConstants.hpp"
#include "RouletteView.hpp"
#include "SpinCountdown.hpp"

using rtype::client::BigWin;
using rtype::client::MotionPreference;
using rtype::client::OtherPlayerBets;
using rtype::client::PLAYER_COLORS;
using rtype::client::RouletteReading;
using rtype::client::rouletteViewOf;
using rtype::client::SpinCountdown;
using rtype::client::testing::A_ROUND;
using rtype::client::testing::ACE_NAME;
using rtype::client::testing::AS_THE_BANNER_LEAVES;
using rtype::client::testing::bettingRoom;
using rtype::client::testing::FIRST_IDLE_FRAME;
using rtype::client::testing::FIRST_OTHER_STACK_OFFSET;
using rtype::client::testing::GAIN_AT_THE_THRESHOLD;
using rtype::client::testing::HELD_GREETING_FRAME;
using rtype::client::testing::INTO_THE_GREETING_OF_A_CYCLE;
using rtype::client::testing::INTO_THE_IDLE_OF_A_CYCLE;
using rtype::client::testing::LUNA_PLACE;
using rtype::client::testing::OTHER_STACK_ALPHA_OF_THE_DESIGN;
using rtype::client::testing::PAYOUT_AT_THE_THRESHOLD;
using rtype::client::testing::PAYOUT_SHORT_OF_THE_THRESHOLD;
using rtype::client::testing::resultRoom;
using rtype::client::testing::SECOND_GREETING_FRAME;
using rtype::client::testing::SECOND_IDLE_FRAME;
using rtype::client::testing::spinningRoom;
using rtype::client::testing::TWELVE_OF_TWENTY_SECONDS;
using rtype::client::testing::TWELVE_SECONDS;
using rtype::client::testing::TWENTY_FIVE_ON_17;
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

/**
 * Given a table where LUNA, seated second, laid a chip, and the player is ACE
 * When it is read
 * Then the stack of LUNA shows among the others, its chips tinted in the
 * second player color, half clear, and moved aside
 */
TEST(RouletteScene, TheBetsOfAnotherPlayerShowTintedInItsColor) {
  CasinoRoomState room = bettingRoom(A_ROUND);
  room.players.at(LUNA_PLACE).bets = {TWENTY_FIVE_ON_17};
  RouletteReading reading;
  reading.room = room;
  reading.ownNickname = std::string{ACE_NAME};
  reading.palette = PLAYER_COLORS;
  sf::Color tint = PLAYER_COLORS.at(LUNA_PLACE).ship;
  tint.a = OTHER_STACK_ALPHA_OF_THE_DESIGN;

  const std::vector<OtherPlayerBets> others = rouletteViewOf(reading).others;

  ASSERT_EQ(others.size(), 1U);
  EXPECT_EQ(others.front().bets, std::vector{TWENTY_FIVE_ON_17});
  EXPECT_EQ(others.front().look.tint, tint);
  EXPECT_EQ(others.front().look.offset, FIRST_OTHER_STACK_OFFSET);
}

/**
 * Given a table where only the player, ACE, laid a chip
 * When it is read
 * Then no other stack shows: the stacks of the player are its own bets
 */
TEST(RouletteScene, TheOwnBetsAreNoOtherStack) {
  CasinoRoomState room = bettingRoom(A_ROUND);
  room.players.front().bets = {TWENTY_FIVE_ON_17};
  RouletteReading reading;
  reading.room = room;
  reading.ownNickname = std::string{ACE_NAME};

  EXPECT_TRUE(rouletteViewOf(reading).others.empty());
}

/**
 * Given a result where ACE gains 2500 chips past its stake
 * When the table is read
 * Then the banner of the big win names ACE and those 2500 chips
 */
TEST(RouletteScene, ABigWinShowsItsBanner) {
  RouletteReading reading;
  reading.room = resultRoom(PAYOUT_AT_THE_THRESHOLD);

  const BigWin banner = rouletteViewOf(reading).bigWin.value_or(BigWin{});

  EXPECT_EQ(banner.nickname, ACE_NAME);
  EXPECT_EQ(banner.gain, GAIN_AT_THE_THRESHOLD);
}

/**
 * Given a result where ACE gains 2499 chips past its stake
 * When the table is read
 * Then no banner shows
 */
TEST(RouletteScene, AWinShortOfTheThresholdShowsNoBanner) {
  RouletteReading reading;
  reading.room = resultRoom(PAYOUT_SHORT_OF_THE_THRESHOLD);

  EXPECT_FALSE(rouletteViewOf(reading).bigWin.has_value());
}

/**
 * Given a result with a big win
 * When 4 s of the result passed
 * Then the banner left the table
 */
TEST(RouletteScene, TheBannerLeavesAfterFourSeconds) {
  RouletteReading reading;
  reading.room = resultRoom(PAYOUT_AT_THE_THRESHOLD);
  reading.room->serverTime = reading.room->resultAt + AS_THE_BANNER_LEAVES;

  EXPECT_FALSE(rouletteViewOf(reading).bigWin.has_value());
}
