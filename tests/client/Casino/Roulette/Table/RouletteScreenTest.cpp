#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <vector>
#include "CasinoRefusal.hpp"
#include "CasinoRoomState.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RouletteScreenFixture.hpp"
#include "RouletteScreenFixtures.hpp"
#include "RouletteScreenTestConstants.hpp"
#include "ScreenTestConstants.hpp"
#include "Wallet.hpp"

using rtype::client::CasinoRefusal;
using rtype::client::testing::A_ROUND;
using rtype::client::testing::A_SECOND_AND_A_HALF;
using rtype::client::testing::ACE_ROW;
using rtype::client::testing::BALL_PICTURE;
using rtype::client::testing::BETS_CLOSE_IN_ELEVEN;
using rtype::client::testing::BETS_CLOSE_IN_TWELVE;
using rtype::client::testing::betsOfAce;
using rtype::client::testing::betsRefused;
using rtype::client::testing::betsTaken;
using rtype::client::testing::bettingRoom;
using rtype::client::testing::CHIPS_FOR_ONE_CHIP_OF_25;
using rtype::client::testing::CHIPS_OF_ACE_TEXT;
using rtype::client::testing::CHIPS_ONCE_A_CHIP_IS_LAID;
using rtype::client::testing::DRAWN_STATUS;
using rtype::client::testing::FIVE_HUNDRED_ON_17;
using rtype::client::testing::FRAME_THAT_BRINGS_17_UP;
using rtype::client::testing::FULL_TABLE_STATUS;
using rtype::client::testing::INSIDE_THE_CHIP_OF_ONE;
using rtype::client::testing::LAST_WORD;
using rtype::client::testing::LOST_STATUS;
using rtype::client::testing::LUNA_NAME;
using rtype::client::testing::NEBULA_LOUNGE_TAG;
using rtype::client::testing::NO_CHIPS;
using rtype::client::testing::NO_CHIPS_STATUS;
using rtype::client::testing::ONE_ON_17;
using rtype::client::testing::PAYOUT_OF_A_STRAIGHT;
using rtype::client::testing::RESULT_IN_FIVE;
using rtype::client::testing::resultRoom;
using rtype::client::testing::ROTOR_TURNS_SHEET;
using rtype::client::testing::SPIN_WORD;
using rtype::client::testing::spinningRoom;
using rtype::client::testing::THE_NEXT_ROUND;
using rtype::client::testing::TOO_FEW_CHIPS_STATUS;
using rtype::client::testing::TWENTY_FIVE_ACROSS_17_AND_20;
using rtype::client::testing::TWENTY_FIVE_ON_17;
using rtype::client::testing::WON_STATUS;
using ::testing::AllOf;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::IsEmpty;
using ::testing::Not;
using Key = sf::Keyboard::Key;

/**
 * Given a table just opened, its bets open
 * When Enter is pressed and released
 * Then a chip of 25, the chip it opens on, is sent on 17, where its cursor
 * opens, for the round in progress
 */
TEST_F(RouletteScreenFixture, EnterLaysTheChosenChipUnderTheCursor) {
  press(Key::Enter);

  EXPECT_THAT(casino().sentBets, ElementsAre(ElementsAre(TWENTY_FIVE_ON_17)));
  EXPECT_THAT(casino().betRounds, ElementsAre(A_ROUND));
}

/**
 * Given a table just opened
 * When 5 is pressed, then Enter
 * Then the chip laid is of 500
 */
TEST_F(RouletteScreenFixture, TheKeyOfAChipChoosesItsValue) {
  press(Key::Num5);

  press(Key::Enter);

  EXPECT_THAT(casino().sentBets, ElementsAre(ElementsAre(FIVE_HUNDRED_ON_17)));
}

/**
 * Given a table just opened
 * When the chip of 1 is clicked, then Enter is pressed
 * Then the chip laid is of 1
 */
TEST_F(RouletteScreenFixture, AClickOnAChipChoosesIt) {
  clickAt(INSIDE_THE_CHIP_OF_ONE);

  press(Key::Enter);

  EXPECT_THAT(casino().sentBets, ElementsAre(ElementsAre(ONE_ON_17)));
}

/**
 * Given a table just opened, its cursor on 17
 * When Right is pressed, then Enter
 * Then the chip lies on the line between 17 and 20
 */
TEST_F(RouletteScreenFixture, TheArrowsMoveWhereTheChipLies) {
  press(Key::Right);

  press(Key::Enter);

  EXPECT_THAT(casino().sentBets,
              ElementsAre(ElementsAre(TWENTY_FIVE_ACROSS_17_AND_20)));
}

/**
 * Given a table just opened
 * When Enter is held down, its press repeated by the system, then released
 * Then one chip is laid
 */
TEST_F(RouletteScreenFixture, AHeldEnterLaysOneChip) {
  hold(Key::Enter);
  hold(Key::Enter);
  release(Key::Enter);

  EXPECT_THAT(casino().sentBets, ElementsAre(ElementsAre(TWENTY_FIVE_ON_17)));
}

/**
 * Given a player holding chips for one chip of 25
 * When Enter is pressed twice
 * Then the second chip is refused: nothing more is sent
 */
TEST_F(RouletteScreenFixture, NoChipPastTheChipsHeld) {
  wallet().balances =
      rtype::network::Wallet{.kopecks = 0, .chips = CHIPS_FOR_ONE_CHIP_OF_25};
  wait(ONE_FRAME);

  press(Key::Enter);
  press(Key::Enter);

  EXPECT_THAT(casino().sentBets, ElementsAre(ElementsAre(TWENTY_FIVE_ON_17)));
}

/**
 * Given a chip on the cloth
 * When C is pressed and released
 * Then the room is sent no bet
 */
TEST_F(RouletteScreenFixture, ClearTakesTheBetsOff) {
  press(Key::Enter);

  press(Key::C);

  ASSERT_FALSE(casino().sentBets.empty());
  EXPECT_THAT(casino().sentBets.back(), IsEmpty());
}

/**
 * Given a chip laid in a round, then the next round open
 * When R is pressed and released
 * Then the chip of the round before is sent again, for the next round
 */
TEST_F(RouletteScreenFixture, RebetLaysTheBetsOfTheRoundBefore) {
  press(Key::Enter);
  casino().roomState = bettingRoom(THE_NEXT_ROUND);
  wait(ONE_FRAME);

  press(Key::R);

  ASSERT_FALSE(casino().sentBets.empty());
  EXPECT_THAT(casino().sentBets.back(), ElementsAre(TWENTY_FIVE_ON_17));
  EXPECT_EQ(casino().betRounds.back(), THE_NEXT_ROUND);
}

/**
 * Given the wheel turning
 * When Enter is pressed and released
 * Then no chip is laid
 */
TEST_F(RouletteScreenFixture, NoChipOnceTheBetsClosed) {
  casino().roomState = spinningRoom();
  wait(ONE_FRAME);

  press(Key::Enter);

  EXPECT_THAT(casino().sentBets, IsEmpty());
}

/**
 * Given a table just opened
 * When Space, the key of the spin there was, is pressed and released
 * Then nothing is sent, and no bar SPIN is painted: the master spins
 */
TEST_F(RouletteScreenFixture, ThePlayerSpinsNothing) {
  press(Key::Space);

  EXPECT_THAT(casino().sentBets, IsEmpty());
  EXPECT_EQ(writingOf(canvas(), SPIN_WORD), nullptr);
}

/**
 * Given a player holding 1 250 chips
 * When a chip of 25 is laid
 * Then the band shows the chips left, 1 225
 */
TEST_F(RouletteScreenFixture, TheBandTakesTheStakeOffTheChips) {
  press(Key::Enter);

  EXPECT_NE(writingOf(canvas(), CHIPS_ONCE_A_CHIP_IS_LAID), nullptr);
}

/**
 * Given a chip of 25 the master took, then the wheel turning while the wallet
 * was not read again
 * When the band is painted
 * Then it shows the chips the master left, not the wallet read before
 */
TEST_F(RouletteScreenFixture, TheBandShowsTheChipsTheMasterLeft) {
  press(Key::Enter);
  casino().betsAnswer = betsTaken({TWENTY_FIVE_ON_17});
  wait(ONE_FRAME);

  casino().roomState = spinningRoom();
  wait(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), CHIPS_ONCE_A_CHIP_IS_LAID), nullptr);
}

/**
 * Given a table whose bets close in 12 s
 * When the clock of the master moves 1.5 s
 * Then the status line counts down from 12 to 11
 */
TEST_F(RouletteScreenFixture, TheStatusCountsDownToTheCloseOfTheBets) {
  EXPECT_NE(writingOf(canvas(), BETS_CLOSE_IN_TWELVE), nullptr);
  rtype::network::CasinoRoomState later = bettingRoom(A_ROUND);
  later.serverTime += A_SECOND_AND_A_HALF;
  casino().roomState = later;

  wait(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), BETS_CLOSE_IN_ELEVEN), nullptr);
}

/**
 * Given the wheel turning, 5 s before it stops
 * When the band is painted
 * Then it counts down to the result
 */
TEST_F(RouletteScreenFixture, TheStatusCountsDownToTheResult) {
  casino().roomState = spinningRoom();

  wait(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), RESULT_IN_FIVE), nullptr);
}

/**
 * Given a player holding no chip
 * When the band is painted
 * Then it sends the player to the cashier
 */
TEST_F(RouletteScreenFixture, NoChipSendsThePlayerToTheCashier) {
  wallet().balances = rtype::network::Wallet{.kopecks = 0, .chips = NO_CHIPS};

  wait(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), NO_CHIPS_STATUS), nullptr);
}

/**
 * Given a chip of 25 laid
 * When the master refuses it for too few chips
 * Then the status line says so, and the chip leaves the cloth
 */
TEST_F(RouletteScreenFixture, RefusedBetsSayWhy) {
  press(Key::Enter);

  casino().betsAnswer = betsRefused(CasinoRefusal::NotEnoughChips);
  wait(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), TOO_FEW_CHIPS_STATUS), nullptr);
  EXPECT_NE(writingOf(canvas(), CHIPS_OF_ACE_TEXT), nullptr);
}

/**
 * Given a room whose seats are all taken
 * When the band is painted
 * Then it says the table is full
 */
TEST_F(RouletteScreenFixture, AFullRoomSaysSo) {
  casino().refusedSeat = CasinoRefusal::RoomFull;

  wait(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), FULL_TABLE_STATUS), nullptr);
}

/**
 * Given 17 drawn, which paid ACE back its straight
 * When the band is painted
 * Then it says the number, its color and the payout
 */
TEST_F(RouletteScreenFixture, TheResultSaysWhatThePlayerWon) {
  casino().roomState = resultRoom(PAYOUT_OF_A_STRAIGHT);
  casino().roundBets = betsOfAce(PAYOUT_OF_A_STRAIGHT);

  wait(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), WON_STATUS), nullptr);
}

/**
 * Given 17 drawn, which paid ACE nothing
 * When the band is painted
 * Then it says the number, its color and the stake lost
 */
TEST_F(RouletteScreenFixture, TheResultSaysWhatThePlayerLost) {
  casino().roomState = resultRoom(0);
  casino().roundBets = betsOfAce(0);

  wait(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), LOST_STATUS), nullptr);
}

/**
 * Given 17 drawn, the bets of ACE not read yet
 * When the band is painted
 * Then it says the number and its color already
 */
TEST_F(RouletteScreenFixture, TheResultShowsBeforeTheBetsAreRead) {
  casino().roomState = resultRoom(0);

  wait(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), DRAWN_STATUS), nullptr);
}

/**
 * Given ACE and LUNA seated
 * When the table is painted
 * Then both are listed by name, ACE as the player
 */
TEST_F(RouletteScreenFixture, TheTableListsThePlayersSeated) {
  EXPECT_NE(writingOf(canvas(), ACE_ROW), nullptr);
  EXPECT_NE(writingOf(canvas(), LUNA_NAME), nullptr);
}

/**
 * Given a table in the Nebula Lounge
 * When the table is painted
 * Then the panel of the players is named after the room
 */
TEST_F(RouletteScreenFixture, ThePanelIsNamedAfterTheRoom) {
  EXPECT_NE(writingOf(canvas(), NEBULA_LOUNGE_TAG), nullptr);
}

/**
 * Given the bets open
 * When the bets close and the wheel turns
 * Then the ball runs, where it was out of play
 */
TEST_F(RouletteScreenFixture, TheBallRunsOnceTheBetsClose) {
  ASSERT_THAT(canvas().images,
              Not(Contains(Field(&RecordedImage::assetId, BALL_PICTURE))));

  casino().roomState = spinningRoom();
  wait(ONE_FRAME);

  EXPECT_THAT(canvas().images,
              Contains(Field(&RecordedImage::assetId, BALL_PICTURE)));
}

/**
 * Given 17 drawn
 * When the wheel is painted
 * Then the rotor shows the frame that brings 17 to the top, where the ball
 * stops
 */
TEST_F(RouletteScreenFixture, TheWheelStopsOnTheWinningPocket) {
  casino().roomState = resultRoom(0);

  wait(ONE_FRAME);

  EXPECT_THAT(
      canvas().images,
      Contains(AllOf(Field(&RecordedImage::assetId, ROTOR_TURNS_SHEET),
                     Field(&RecordedImage::source, FRAME_THAT_BRINGS_17_UP))));
}

/**
 * Given a room that drew no number before
 * When its number is drawn
 * Then the numbers drawn show
 */
TEST_F(RouletteScreenFixture, TheNumbersDrawnShowOnceOneWas) {
  ASSERT_EQ(writingOf(canvas(), LAST_WORD), nullptr);

  casino().roomState = resultRoom(0);
  wait(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), LAST_WORD), nullptr);
}

/**
 * Given a table just opened
 * When Escape is pressed and released
 * Then the table closes, back to the hall
 */
TEST_F(RouletteScreenFixture, BackClosesTheTable) {
  press(Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}
