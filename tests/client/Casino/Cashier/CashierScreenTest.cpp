#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "CashierScreenFixture.hpp"
#include "CashierScreenTestConstants.hpp"
#include "CashierWay.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"

using rtype::client::CashierWay;
using rtype::client::testing::CHIPS_AFTER_BUYING;
using rtype::client::testing::CHIPS_AFTER_BUYING_500;
using rtype::client::testing::CHIPS_AFTER_CASHING_OUT_500;
using rtype::client::testing::CHIPS_HELD;
using rtype::client::testing::FIRST_AMOUNT;
using rtype::client::testing::INSIDE_THE_CHIP_OF_ONE;
using rtype::client::testing::INSIDE_THE_RAISE_MARK;
using rtype::client::testing::KOPECKS_AFTER_BUYING_500;
using rtype::client::testing::KOPECKS_AFTER_CASHING_OUT_500;
using rtype::client::testing::KOPECKS_HELD;
using rtype::client::testing::KOPECKS_LEFT_AFTER_BUYING;
using rtype::client::testing::ONE_LARGEST_STEP_HIGHER;
using rtype::client::testing::ONE_STEP_HIGHER;
using rtype::client::testing::ONE_STEP_LOWER;
using rtype::client::testing::SEVEN_HUNDRED_FIFTY_KOPECKS_MISSING;
using rtype::client::testing::STEP_OF_1;
using rtype::client::testing::STEP_OF_100;
using rtype::client::testing::STEP_OF_500;
using rtype::client::testing::STEPS_TO_FIVE_THOUSAND;
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
 * Then 500 kopecks become 500 chips, the settings are saved once, and the
 * cashier closes
 */
TEST_F(CashierScreenFixture, EnterBuysTheChipsAndSaves) {
  press(Key::Enter);

  EXPECT_EQ(settings().kopecks, KOPECKS_AFTER_BUYING_500);
  EXPECT_EQ(settings().chips, CHIPS_AFTER_BUYING_500);
  ASSERT_EQ(storage().saves().size(), 1U);
  EXPECT_EQ(storage().saves().back().chips, CHIPS_AFTER_BUYING_500);
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given a cashier that cashes out, 500 offered
 * When Enter is pressed and released
 * Then 500 chips become 500 kopecks
 */
TEST_F(CashierScreenFixture, EnterCashesTheChipsOut) {
  reopenWith(CashierWay::CashOut);

  press(Key::Enter);

  EXPECT_EQ(settings().kopecks, KOPECKS_AFTER_CASHING_OUT_500);
  EXPECT_EQ(settings().chips, CHIPS_AFTER_CASHING_OUT_500);
}

/**
 * Given a cashier that buys chips
 * When Enter is held down, its press repeated by the system, then released
 * Then the chips are bought once
 */
TEST_F(CashierScreenFixture, AHeldEnterBuysOnce) {
  hold(Key::Enter);
  hold(Key::Enter);
  release(Key::Enter);

  EXPECT_EQ(settings().kopecks, KOPECKS_AFTER_BUYING_500);
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
 * Then nothing is changed nor saved, and the cashier stays open
 */
TEST_F(CashierScreenFixture, ARefusedAmountChangesNothing) {
  press(Key::Num5);
  for (int step = 0; step < STEPS_TO_FIVE_THOUSAND; ++step) {
    press(Key::Right);
  }

  press(Key::Enter);

  EXPECT_EQ(settings().kopecks, KOPECKS_HELD);
  EXPECT_EQ(settings().chips, CHIPS_HELD);
  EXPECT_TRUE(storage().saves().empty());
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given a cashier
 * When Escape is pressed and released
 * Then it closes, nothing changed
 */
TEST_F(CashierScreenFixture, BackClosesWithoutExchanging) {
  press(Key::Escape);

  EXPECT_EQ(settings().kopecks, KOPECKS_HELD);
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}
