#include <gtest/gtest.h>
#include "CashierRules.hpp"
#include "CashierRulesTestConstants.hpp"
#include "CashierWay.hpp"

using rtype::client::cashierLineOf;
using rtype::client::CashierWay;
using rtype::client::exchangeAt;
using rtype::client::testing::A_PURSE;
using rtype::client::testing::CHOOSE_AN_AMOUNT;
using rtype::client::testing::FULL_CHIPS;
using rtype::client::testing::NO_AMOUNT;
using rtype::client::testing::NO_ROOM_FOR_MORE_CHIPS;
using rtype::client::testing::ONE_CHIP;
using rtype::client::testing::TWO_HUNDRED_FIFTY_CHIPS_MISSING;
using rtype::client::testing::TWO_HUNDRED_FIFTY_CHIPS_TOO_MANY;

/**
 * Given a purse
 * When no chip is bought
 * Then the cashier asks for an amount
 */
TEST(CashierRules, NoAmountAsksForOne) {
  EXPECT_EQ(cashierLineOf(exchangeAt(A_PURSE, CashierWay::BuyChips, NO_AMOUNT),
                          CashierWay::BuyChips, NO_AMOUNT),
            CHOOSE_AN_AMOUNT);
}

/**
 * Given a purse of 1 250 chips
 * When 1 500 are cashed out
 * Then the cashier says 250 chips are missing
 */
TEST(CashierRules, CashingOutMoreThanHeldSaysWhatIsMissing) {
  EXPECT_EQ(
      cashierLineOf(exchangeAt(A_PURSE, CashierWay::CashOut,
                               TWO_HUNDRED_FIFTY_CHIPS_TOO_MANY),
                    CashierWay::CashOut, TWO_HUNDRED_FIFTY_CHIPS_TOO_MANY),
      TWO_HUNDRED_FIFTY_CHIPS_MISSING);
}

/**
 * Given a purse whose chips are full
 * When one more chip is bought
 * Then the cashier says the chips have no room left
 */
TEST(CashierRules, BuyingIntoFullChipsSaysThereIsNoRoom) {
  EXPECT_EQ(
      cashierLineOf(exchangeAt(FULL_CHIPS, CashierWay::BuyChips, ONE_CHIP),
                    CashierWay::BuyChips, ONE_CHIP),
      NO_ROOM_FOR_MORE_CHIPS);
}

/**
 * Given a purse
 * When an amount it holds is bought
 * Then the cashier says nothing against it
 */
TEST(CashierRules, AnExchangeTheCashierMakesHasNoLine) {
  EXPECT_TRUE(cashierLineOf(exchangeAt(A_PURSE, CashierWay::BuyChips, ONE_CHIP),
                            CashierWay::BuyChips, ONE_CHIP)
                  .empty());
}
