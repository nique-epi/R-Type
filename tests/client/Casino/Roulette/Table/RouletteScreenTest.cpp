#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <vector>
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RouletteScreenFixture.hpp"
#include "RouletteScreenFixtures.hpp"
#include "RouletteScreenTestConstants.hpp"

using rtype::client::testing::A_SECOND_AND_A_HALF;
using rtype::client::testing::ACE_ROW;
using rtype::client::testing::BETS_CLOSE_IN_ELEVEN;
using rtype::client::testing::BETS_CLOSE_IN_TWELVE;
using rtype::client::testing::bettingRound;
using rtype::client::testing::CHIPS_FOR_ONE_CHIP_OF_25;
using rtype::client::testing::CHIPS_ONCE_A_CHIP_IS_LAID;
using rtype::client::testing::CHIPS_ONCE_SETTLED;
using rtype::client::testing::FIVE_HUNDRED_ON_17;
using rtype::client::testing::FRAME_THAT_BRINGS_17_UP;
using rtype::client::testing::INSIDE_THE_CHIP_OF_ONE;
using rtype::client::testing::LAST_WORD;
using rtype::client::testing::LOST_STATUS;
using rtype::client::testing::LUNA_NAME;
using rtype::client::testing::NO_CHIPS;
using rtype::client::testing::NO_CHIPS_STATUS;
using rtype::client::testing::NO_MORE_BETS_STATUS;
using rtype::client::testing::ONE_ON_17;
using rtype::client::testing::PAYOUT_OF_A_STRAIGHT;
using rtype::client::testing::resultRound;
using rtype::client::testing::ROTOR_TURNS_SHEET;
using rtype::client::testing::spinningRound;
using rtype::client::testing::THE_NEXT_ROUND;
using rtype::client::testing::TWENTY_FIVE_ACROSS_17_AND_20;
using rtype::client::testing::TWENTY_FIVE_ON_17;
using rtype::client::testing::WON_STATUS;
using ::testing::AllOf;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::IsEmpty;
using Key = sf::Keyboard::Key;

/**
 * Given a table just opened, its bets open
 * When Enter is pressed and released
 * Then a chip of 25, the chip it opens on, is sent on 17, where its cursor
 * opens, with no vote
 */
TEST_F(RouletteScreenFixture, EnterLaysTheChosenChipUnderTheCursor) {
  press(Key::Enter);

  EXPECT_THAT(casino().sentBets(), ElementsAre(ElementsAre(TWENTY_FIVE_ON_17)));
  EXPECT_THAT(casino().spinVotes(), ElementsAre(false));
}

/**
 * Given a table just opened
 * When 5 is pressed, then Enter
 * Then the chip laid is of 500
 */
TEST_F(RouletteScreenFixture, TheKeyOfAChipChoosesItsValue) {
  press(Key::Num5);

  press(Key::Enter);

  EXPECT_THAT(casino().sentBets(),
              ElementsAre(ElementsAre(FIVE_HUNDRED_ON_17)));
}

/**
 * Given a table just opened
 * When the chip of 1 is clicked, then Enter is pressed
 * Then the chip laid is of 1
 */
TEST_F(RouletteScreenFixture, AClickOnAChipChoosesIt) {
  clickAt(INSIDE_THE_CHIP_OF_ONE);

  press(Key::Enter);

  EXPECT_THAT(casino().sentBets(), ElementsAre(ElementsAre(ONE_ON_17)));
}

/**
 * Given a table just opened, its cursor on 17
 * When Right is pressed, then Enter
 * Then the chip lies on the line between 17 and 20
 */
TEST_F(RouletteScreenFixture, TheArrowsMoveWhereTheChipLies) {
  press(Key::Right);

  press(Key::Enter);

  EXPECT_THAT(casino().sentBets(),
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

  EXPECT_THAT(casino().sentBets(), ElementsAre(ElementsAre(TWENTY_FIVE_ON_17)));
}

/**
 * Given a player holding chips for one chip of 25
 * When Enter is pressed twice
 * Then the second chip is refused: nothing more is sent
 */
TEST_F(RouletteScreenFixture, NoChipPastTheChipsHeld) {
  holdChips(CHIPS_FOR_ONE_CHIP_OF_25);

  press(Key::Enter);
  press(Key::Enter);

  EXPECT_THAT(casino().sentBets(), ElementsAre(ElementsAre(TWENTY_FIVE_ON_17)));
}

/**
 * Given a chip on the cloth
 * When C is pressed and released
 * Then the table is sent no bet
 */
TEST_F(RouletteScreenFixture, ClearTakesTheBetsOff) {
  press(Key::Enter);

  press(Key::C);

  ASSERT_FALSE(casino().sentBets().empty());
  EXPECT_THAT(casino().sentBets().back(), IsEmpty());
}

/**
 * Given a chip laid in a round, then the next round open
 * When R is pressed and released
 * Then the chip of the round before is sent again
 */
TEST_F(RouletteScreenFixture, RebetLaysTheBetsOfTheRoundBefore) {
  press(Key::Enter);
  showRound(bettingRound(THE_NEXT_ROUND, 0));

  press(Key::R);

  ASSERT_FALSE(casino().sentBets().empty());
  EXPECT_THAT(casino().sentBets().back(), ElementsAre(TWENTY_FIVE_ON_17));
}

/**
 * Given a chip on the cloth
 * When Space is pressed and released
 * Then the bets are sent again with the vote to spin
 */
TEST_F(RouletteScreenFixture, SpinSendsTheVoteWithTheBets) {
  press(Key::Enter);

  press(Key::Space);

  EXPECT_THAT(casino().spinVotes(), ElementsAre(false, true));
  EXPECT_THAT(casino().sentBets().back(), ElementsAre(TWENTY_FIVE_ON_17));
}

/**
 * Given a table just opened
 * When Space is held down, its press repeated by the system, then released
 * Then the vote is sent once
 */
TEST_F(RouletteScreenFixture, AHeldSpaceVotesOnce) {
  hold(Key::Space);
  hold(Key::Space);
  release(Key::Space);

  EXPECT_THAT(casino().spinVotes(), ElementsAre(true));
}

/**
 * Given the wheel turning
 * When Enter is pressed and released
 * Then no chip is laid
 */
TEST_F(RouletteScreenFixture, NoChipOnceTheBetsClosed) {
  showRound(spinningRound());

  press(Key::Enter);

  EXPECT_THAT(casino().sentBets(), IsEmpty());
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
 * Given a chip of 25 laid, then the wheel turning and the round settled in
 * the settings
 * When the band is painted
 * Then it shows the chips held when the bets closed, not the settled ones
 */
TEST_F(RouletteScreenFixture, TheBandKeepsTheChipsUntilTheResult) {
  press(Key::Enter);
  showRound(spinningRound());

  holdChips(CHIPS_ONCE_SETTLED);

  EXPECT_NE(writingOf(canvas(), CHIPS_ONCE_A_CHIP_IS_LAID), nullptr);
}

/**
 * Given a table whose bets close in 12 s
 * When 1.5 s pass with no new state of the table
 * Then the status line counts down from 12 to 11
 */
TEST_F(RouletteScreenFixture, TheStatusCountsDownToTheCloseOfTheBets) {
  EXPECT_NE(writingOf(canvas(), BETS_CLOSE_IN_TWELVE), nullptr);

  wait(A_SECOND_AND_A_HALF);

  EXPECT_NE(writingOf(canvas(), BETS_CLOSE_IN_ELEVEN), nullptr);
}

/**
 * Given a player holding no chip
 * When the band is painted
 * Then it sends the player to the cashier
 */
TEST_F(RouletteScreenFixture, NoChipSendsThePlayerToTheCashier) {
  holdChips(NO_CHIPS);

  EXPECT_NE(writingOf(canvas(), NO_CHIPS_STATUS), nullptr);
}

/**
 * Given the wheel turning
 * When the band is painted
 * Then it says no more bets
 */
TEST_F(RouletteScreenFixture, NoMoreBetsWhileTheWheelTurns) {
  showRound(spinningRound());

  EXPECT_NE(writingOf(canvas(), NO_MORE_BETS_STATUS), nullptr);
}

/**
 * Given 17 drawn, which paid ACE back its straight
 * When the band is painted
 * Then it says the number, its color and the payout
 */
TEST_F(RouletteScreenFixture, TheResultSaysWhatThePlayerWon) {
  showRound(resultRound(PAYOUT_OF_A_STRAIGHT));

  EXPECT_NE(writingOf(canvas(), WON_STATUS), nullptr);
}

/**
 * Given 17 drawn, which paid ACE nothing
 * When the band is painted
 * Then it says the number, its color and the stake lost
 */
TEST_F(RouletteScreenFixture, TheResultSaysWhatThePlayerLost) {
  showRound(resultRound(0));

  EXPECT_NE(writingOf(canvas(), LOST_STATUS), nullptr);
}

/**
 * Given ACE and LUNA seated
 * When the table is painted
 * Then both are listed by name, ACE as the player
 */
TEST_F(RouletteScreenFixture, TheTableListsThePilotsSeated) {
  EXPECT_NE(writingOf(canvas(), ACE_ROW), nullptr);
  EXPECT_NE(writingOf(canvas(), LUNA_NAME), nullptr);
}

/**
 * Given 17 drawn
 * When the wheel is painted
 * Then the rotor shows the frame that brings 17 to the top, where the ball
 * stops
 */
TEST_F(RouletteScreenFixture, TheWheelStopsOnTheWinningPocket) {
  showRound(resultRound(0));

  EXPECT_THAT(
      canvas().images,
      Contains(AllOf(Field(&RecordedImage::assetId, ROTOR_TURNS_SHEET),
                     Field(&RecordedImage::source, FRAME_THAT_BRINGS_17_UP))));
}

/**
 * Given a table that saw no number drawn, then a result
 * When the table is painted
 * Then the numbers drawn show once one was
 */
TEST_F(RouletteScreenFixture, TheNumbersDrawnShowOnceOneWas) {
  ASSERT_EQ(writingOf(canvas(), LAST_WORD), nullptr);

  showRound(resultRound(0));

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
