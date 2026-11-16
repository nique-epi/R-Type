#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "CashierConstants.hpp"
#include "CashierScreenFixture.hpp"
#include "CashierScreenTestConstants.hpp"
#include "CashierWay.hpp"
#include "Currency.hpp"
#include "ExchangeRequest.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "Wallet.hpp"
#include "WalletChangeReading.hpp"
#include "WalletRefusal.hpp"

using rtype::client::CashierWay;
using rtype::client::MASTER_SILENT_LINE;
using rtype::client::WalletChangeReading;
using rtype::client::WalletRefusal;
using rtype::client::testing::AMOUNT_OFFERED;
using rtype::client::testing::CHIPS_AFTER_BUYING;
using rtype::client::testing::CHIPS_AFTER_BUYING_500;
using rtype::client::testing::FIRST_AMOUNT;
using rtype::client::testing::INSIDE_THE_CHIP_OF_ONE;
using rtype::client::testing::INSIDE_THE_RAISE_MARK;
using rtype::client::testing::KOPECKS_AFTER_BUYING_500;
using rtype::client::testing::KOPECKS_LEFT_AFTER_BUYING;
using rtype::client::testing::ONE_LARGEST_STEP_HIGHER;
using rtype::client::testing::ONE_STEP_HIGHER;
using rtype::client::testing::ONE_STEP_LOWER;
using rtype::client::testing::SEVEN_HUNDRED_FIFTY_KOPECKS_MISSING;
using rtype::client::testing::STEP_OF_1;
using rtype::client::testing::STEP_OF_100;
using rtype::client::testing::STEP_OF_500;
using rtype::client::testing::STEPS_TO_FIVE_THOUSAND;
using rtype::network::Currency;
using rtype::network::ExchangeRequest;
using rtype::network::Wallet;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using Key = sf::Keyboard::Key;

/**
 * Given a cashier just opened
 * When it is painted
 * Then it offers 500, by steps of 100, and what both balances would hold
 * after buying them
 */
TEST_F(CashierScreenFixture, OpensOnFiveHundredByStepsOfOneHundred) {
  EXPECT_NE(writingOf(canvas(), FIRST_AMOUNT), nullptr);
  EXPECT_NE(writingOf(canvas(), STEP_OF_100), nullptr);
  EXPECT_NE(writingOf(canvas(), KOPECKS_LEFT_AFTER_BUYING), nullptr);
  EXPECT_NE(writingOf(canvas(), CHIPS_AFTER_BUYING), nullptr);
}

/**
 * Given a cashier just opened
 * When Right is pressed and released
 * Then the amount rises by the step
 */
TEST_F(CashierScreenFixture, RightRaisesTheAmountByTheStep) {
  press(Key::Right);

  EXPECT_NE(writingOf(canvas(), ONE_STEP_HIGHER), nullptr);
}

/**
 * Given a cashier just opened
 * When Left is pressed and released
 * Then the amount falls by the step
 */
TEST_F(CashierScreenFixture, LeftLowersTheAmountByTheStep) {
  press(Key::Left);

  EXPECT_NE(writingOf(canvas(), ONE_STEP_LOWER), nullptr);
}

/**
 * Given a cashier just opened
 * When 5 chooses the chip of 500, then Right is pressed and released
 * Then the step is 500, and the amount rose by it
 */
TEST_F(CashierScreenFixture, TheKeyOfAChipChoosesTheStep) {
  press(Key::Num5);

  press(Key::Right);

  EXPECT_NE(writingOf(canvas(), STEP_OF_500), nullptr);
  EXPECT_NE(writingOf(canvas(), ONE_LARGEST_STEP_HIGHER), nullptr);
}

/**
 * Given a cashier just opened
 * When the chip of 1 is clicked
 * Then the step is 1
 */
TEST_F(CashierScreenFixture, AClickOnAChipChoosesTheStep) {
  clickAt(INSIDE_THE_CHIP_OF_ONE);

  EXPECT_NE(writingOf(canvas(), STEP_OF_1), nullptr);
}

/**
 * Given a cashier just opened
 * When the mark on the right of the amount is clicked
 * Then the amount rises by the step
 */
TEST_F(CashierScreenFixture, AClickOnTheRaiseMarkRaisesTheAmount) {
  clickAt(INSIDE_THE_RAISE_MARK);

  EXPECT_NE(writingOf(canvas(), ONE_STEP_HIGHER), nullptr);
}

/**
 * Given a cashier that buys chips, 500 offered
 * When Enter is pressed and released
 * Then the master is asked to change 500 kopecks into chips, and the cashier
 * waits for its answer
 */
TEST_F(CashierScreenFixture, EnterAsksTheMasterForTheChips) {
  press(Key::Enter);

  EXPECT_THAT(wallet().exchangesAsked,
              ElementsAre(ExchangeRequest{.from = Currency::Kopecks,
                                          .amount = AMOUNT_OFFERED}));
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given a cashier that asked the master for 500 chips
 * When the master answers that it made the exchange
 * Then the cashier closes
 */
TEST_F(CashierScreenFixture, ClosesOnceTheMasterMadeTheExchange) {
  press(Key::Enter);

  wallet().exchangeAnswer =
      WalletChangeReading{.ask = wallet().exchangesAsked.size(),
                          .wallet = Wallet{.kopecks = KOPECKS_AFTER_BUYING_500,
                                           .chips = CHIPS_AFTER_BUYING_500},
                          .refusal = std::nullopt};
  wait();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given a cashier that asked the master for 500 chips
 * When the master answers that it could not be reached
 * Then the cashier stays open and says no answer came
 */
TEST_F(CashierScreenFixture, AnExchangeTheMasterDidNotMakeSaysWhy) {
  press(Key::Enter);

  wallet().exchangeAnswer =
      WalletChangeReading{.ask = wallet().exchangesAsked.size(),
                          .wallet = std::nullopt,
                          .refusal = WalletRefusal::MasterUnreachable};
  wait();

  EXPECT_THAT(navigator().calls(), IsEmpty());
  EXPECT_NE(writingOf(canvas(), MASTER_SILENT_LINE), nullptr);
}

/**
 * Given a cashier that cashes out, 500 offered
 * When Enter is pressed and released
 * Then the master is asked to change 500 chips back into kopecks
 */
TEST_F(CashierScreenFixture, EnterCashesTheChipsOut) {
  reopenWith(CashierWay::CashOut);

  press(Key::Enter);

  EXPECT_THAT(wallet().exchangesAsked,
              ElementsAre(ExchangeRequest{.from = Currency::Chips,
                                          .amount = AMOUNT_OFFERED}));
}

/**
 * Given a cashier that buys chips
 * When Enter is held down, its press repeated by the system, then released
 * Then the master is asked once
 */
TEST_F(CashierScreenFixture, AHeldEnterBuysOnce) {
  hold(Key::Enter);
  hold(Key::Enter);
  release(Key::Enter);

  EXPECT_EQ(wallet().exchangesAsked.size(), 1U);
}

/**
 * Given a cashier asked for 5 000 chips, 750 more than the kopecks held
 * When it is painted
 * Then it says how many kopecks are missing
 */
TEST_F(CashierScreenFixture, AnAmountPastTheKopecksSaysWhatIsMissing) {
  press(Key::Num5);
  for (int step = 0; step < STEPS_TO_FIVE_THOUSAND; ++step) {
    press(Key::Right);
  }

  EXPECT_NE(writingOf(canvas(), SEVEN_HUNDRED_FIFTY_KOPECKS_MISSING), nullptr);
}

/**
 * Given a cashier asked for 5 000 chips, 750 more than the kopecks held
 * When Enter is pressed and released
 * Then nothing is asked of the master, and the cashier stays open
 */
TEST_F(CashierScreenFixture, ARefusedAmountChangesNothing) {
  press(Key::Num5);
  for (int step = 0; step < STEPS_TO_FIVE_THOUSAND; ++step) {
    press(Key::Right);
  }

  press(Key::Enter);

  EXPECT_TRUE(wallet().exchangesAsked.empty());
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given a cashier
 * When Escape is pressed and released
 * Then it closes, nothing changed
 */
TEST_F(CashierScreenFixture, BackClosesWithoutExchanging) {
  press(Key::Escape);

  EXPECT_TRUE(wallet().exchangesAsked.empty());
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}
