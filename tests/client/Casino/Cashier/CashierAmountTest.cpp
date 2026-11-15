#include <gtest/gtest.h>
#include "CashierAmount.hpp"
#include "CashierAmountTestConstants.hpp"
#include "SettingsConstants.hpp"

using rtype::client::CashierAmount;
using rtype::client::MAXIMUM_CHIPS;
using rtype::client::testing::MORE_STEPS_THAN_THE_LARGEST_BALANCE;
using rtype::client::testing::OPENING_AMOUNT;
using rtype::client::testing::OPENING_STEP_PLACE;
using rtype::client::testing::PAST_THE_LAST_STEP;
using rtype::client::testing::STEP_OF_FIVE_HUNDRED_PLACE;
using rtype::client::testing::STEP_OF_ONE_PLACE;

/**
 * Given an amount at 500, by steps of 500
 * When it is lowered twice
 * Then it holds none, never less
 */
TEST(CashierAmount, LoweringNeverFallsBelowNone) {
  CashierAmount amount;
  amount.chooseStep(STEP_OF_FIVE_HUNDRED_PLACE);

  amount.lower();
  amount.lower();

  EXPECT_EQ(amount.amount(), 0U);
}

/**
 * Given an amount by steps of 500
 * When it is raised more times than the largest balance holds steps
 * Then it holds the largest balance, never more
 */
TEST(CashierAmount, RaisingStopsAtTheLargestBalance) {
  CashierAmount amount;
  amount.chooseStep(STEP_OF_FIVE_HUNDRED_PLACE);

  for (int step = 0; step < MORE_STEPS_THAN_THE_LARGEST_BALANCE; ++step) {
    amount.raise();
  }

  EXPECT_EQ(amount.amount(), MAXIMUM_CHIPS);
}

/**
 * Given a new amount
 * When a step past the last is chosen
 * Then the step stays where it was
 */
TEST(CashierAmount, AStepPastTheLastChangesNothing) {
  CashierAmount amount;

  amount.chooseStep(PAST_THE_LAST_STEP);

  EXPECT_EQ(amount.step(), OPENING_STEP_PLACE);
}

/**
 * Given a new amount
 * When the step of 1 is chosen, then the amount raised
 * Then it rose by one
 */
TEST(CashierAmount, TheChosenStepMovesTheAmount) {
  CashierAmount amount;
  amount.chooseStep(STEP_OF_ONE_PLACE);

  amount.raise();

  EXPECT_EQ(amount.amount(), OPENING_AMOUNT + 1);
}
