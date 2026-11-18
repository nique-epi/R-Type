#include <gtest/gtest.h>
#include "CashierRules.hpp"
#include "CashierRulesTestConstants.hpp"
#include "CashierWay.hpp"
#include "WalletRefusal.hpp"

using rtype::client::cashierLineOf;
using rtype::client::cashierRefusalLineOf;
using rtype::client::CashierWay;
using rtype::client::exchangeAt;
using rtype::client::WalletRefusal;
using rtype::client::testing::A_WALLET;
using rtype::client::testing::CHOOSE_AN_AMOUNT;
using rtype::client::testing::FULL_CHIPS;
using rtype::client::testing::NO_AMOUNT;
using rtype::client::testing::NO_ROOM_FOR_MORE_CHIPS;
using rtype::client::testing::ONE_CHIP;
using rtype::client::testing::TWO_HUNDRED_FIFTY_CHIPS_MISSING;
using rtype::client::testing::TWO_HUNDRED_FIFTY_CHIPS_TOO_MANY;
using rtype::client::testing::TWO_HUNDRED_FIFTY_KOPECKS_MISSING;
using rtype::client::testing::TWO_HUNDRED_FIFTY_KOPECKS_TOO_MANY;

/**
 * Given a wallet
 * When no chip is bought
 * Then the cashier asks for an amount
 */
TEST(CashierRules, NoAmountAsksForOne) {
  EXPECT_EQ(cashierLineOf(exchangeAt(A_WALLET, CashierWay::BuyChips, NO_AMOUNT),
                          CashierWay::BuyChips, NO_AMOUNT),
            CHOOSE_AN_AMOUNT);
}

/**
 * Given a wallet of 1 250 chips
 * When 1 500 are cashed out
 * Then the cashier says 250 chips are missing
 */
TEST(CashierRules, CashingOutMoreThanHeldSaysWhatIsMissing) {
  EXPECT_EQ(
      cashierLineOf(exchangeAt(A_WALLET, CashierWay::CashOut,
                               TWO_HUNDRED_FIFTY_CHIPS_TOO_MANY),
                    CashierWay::CashOut, TWO_HUNDRED_FIFTY_CHIPS_TOO_MANY),
      TWO_HUNDRED_FIFTY_CHIPS_MISSING);
}

/**
 * Given a wallet of 4 250 kopecks
 * When 4 500 chips are bought
 * Then the cashier says 250 kopecks are missing
 */
TEST(CashierRules, BuyingMoreThanHeldSaysWhatIsMissing) {
  EXPECT_EQ(
      cashierLineOf(exchangeAt(A_WALLET, CashierWay::BuyChips,
                               TWO_HUNDRED_FIFTY_KOPECKS_TOO_MANY),
                    CashierWay::BuyChips, TWO_HUNDRED_FIFTY_KOPECKS_TOO_MANY),
      TWO_HUNDRED_FIFTY_KOPECKS_MISSING);
}

/**
 * Given a wallet of 1 250 chips
 * When 1 500 are cashed out
 * Then the exchange tried leaves the wallet as it was
 */
TEST(CashierRules, ARefusedExchangeLeavesTheWallet) {
  EXPECT_EQ(exchangeAt(A_WALLET, CashierWay::CashOut,
                       TWO_HUNDRED_FIFTY_CHIPS_TOO_MANY)
                .wallet,
            A_WALLET);
}

/**
 * Given a wallet whose chips are full
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
 * Given a wallet
 * When an amount it holds is bought
 * Then the cashier says nothing against it
 */
TEST(CashierRules, AnExchangeTheCashierMakesHasNoLine) {
  EXPECT_TRUE(
      cashierLineOf(exchangeAt(A_WALLET, CashierWay::BuyChips, ONE_CHIP),
                    CashierWay::BuyChips, ONE_CHIP)
          .empty());
}

/**
 * Given the master refusing an exchange for its amount
 * When the cashier says why
 * Then it asks for an amount
 */
TEST(CashierRules, AnAmountTheMasterRefusesAsksForOne) {
  EXPECT_EQ(cashierRefusalLineOf(WalletRefusal::InvalidAmount),
            CHOOSE_AN_AMOUNT);
}
