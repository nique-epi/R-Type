#include <gtest/gtest.h>
#include <optional>
#include "ChipsExchange.hpp"
#include "ChipsExchangeTestConstants.hpp"
#include "Exchange.hpp"
#include "ExchangeRefusal.hpp"
#include "KopecksTestConstants.hpp"
#include "Purse.hpp"

using rtype::client::buyChips;
using rtype::client::cashOutChips;
using rtype::client::Exchange;
using rtype::client::ExchangeRefusal;
using rtype::client::Purse;
using rtype::client::testing::AN_AMOUNT;
using rtype::client::testing::AN_AMOUNT_SHORT_OF_THE_LARGEST;
using rtype::client::testing::CHIPS_AFTER_BUYING;
using rtype::client::testing::CHIPS_AFTER_CASHING_OUT;
using rtype::client::testing::KOPECKS_AFTER_BUYING;
using rtype::client::testing::KOPECKS_AFTER_CASHING_OUT;
using rtype::client::testing::LARGEST_BALANCE;
using rtype::client::testing::SOME_CHIPS;
using rtype::client::testing::SOME_KOPECKS;

/**
 * Given a purse of 4 250 kopecks and 1 250 chips
 * When it buys 500 chips
 * Then the cashier takes 500 kopecks and gives 500 chips
 */
TEST(ChipsExchange, BuyingChangesKopecksIntoAsManyChips) {
  const Purse purse{.kopecks = SOME_KOPECKS, .chips = SOME_CHIPS, .locker = {}};

  const Exchange bought = buyChips(purse, AN_AMOUNT);

  EXPECT_EQ(bought.refusal, std::nullopt);
  EXPECT_EQ(bought.purse.kopecks, KOPECKS_AFTER_BUYING);
  EXPECT_EQ(bought.purse.chips, CHIPS_AFTER_BUYING);
}

/**
 * Given a purse of 4 250 kopecks and 1 250 chips
 * When it cashes out 500 chips
 * Then the cashier takes 500 chips and gives 500 kopecks
 */
TEST(ChipsExchange, CashingOutChangesChipsIntoAsManyKopecks) {
  const Purse purse{.kopecks = SOME_KOPECKS, .chips = SOME_CHIPS, .locker = {}};

  const Exchange cashed = cashOutChips(purse, AN_AMOUNT);

  EXPECT_EQ(cashed.refusal, std::nullopt);
  EXPECT_EQ(cashed.purse.kopecks, KOPECKS_AFTER_CASHING_OUT);
  EXPECT_EQ(cashed.purse.chips, CHIPS_AFTER_CASHING_OUT);
}

/**
 * Given a purse of 4 250 kopecks and 1 250 chips
 * When it buys 500 chips, then cashes the same 500 out
 * Then it holds what it held: the cashier keeps no fee
 */
TEST(ChipsExchange, ARoundTripKeepsThePurse) {
  const Purse purse{.kopecks = SOME_KOPECKS, .chips = SOME_CHIPS, .locker = {}};

  const Exchange back =
      cashOutChips(buyChips(purse, AN_AMOUNT).purse, AN_AMOUNT);

  EXPECT_EQ(back.refusal, std::nullopt);
  EXPECT_EQ(back.purse, purse);
}

/**
 * Given a purse of 4 250 kopecks
 * When it buys exactly 4 250 chips
 * Then the cashier changes the whole balance and leaves no kopeck
 */
TEST(ChipsExchange, BuyingTheWholeBalanceIsAccepted) {
  const Purse purse{.kopecks = SOME_KOPECKS, .chips = 0, .locker = {}};

  const Exchange bought = buyChips(purse, SOME_KOPECKS);

  EXPECT_EQ(bought.refusal, std::nullopt);
  EXPECT_EQ(bought.purse.kopecks, 0U);
  EXPECT_EQ(bought.purse.chips, SOME_KOPECKS);
}

/**
 * Given a purse of 4 250 kopecks
 * When it buys 4 251 chips
 * Then the cashier refuses, not enough kopecks, and the purse is as it was
 */
TEST(ChipsExchange, BuyingMoreThanTheKopecksIsRefused) {
  const Purse purse{.kopecks = SOME_KOPECKS, .chips = SOME_CHIPS, .locker = {}};

  const Exchange refused = buyChips(purse, SOME_KOPECKS + 1);

  EXPECT_EQ(refused.refusal, ExchangeRefusal::NotEnoughKopecks);
  EXPECT_EQ(refused.purse, purse);
}

/**
 * Given a purse of 1 250 chips
 * When it cashes out exactly 1 250 chips
 * Then the cashier changes them all and leaves no chip
 */
TEST(ChipsExchange, CashingOutEveryChipIsAccepted) {
  const Purse purse{.kopecks = 0, .chips = SOME_CHIPS, .locker = {}};

  const Exchange cashed = cashOutChips(purse, SOME_CHIPS);

  EXPECT_EQ(cashed.refusal, std::nullopt);
  EXPECT_EQ(cashed.purse.chips, 0U);
  EXPECT_EQ(cashed.purse.kopecks, SOME_CHIPS);
}

/**
 * Given a purse of 1 250 chips
 * When it cashes out 1 251 chips
 * Then the cashier refuses, not enough chips, and the purse is as it was
 */
TEST(ChipsExchange, CashingOutMoreThanTheChipsIsRefused) {
  const Purse purse{.kopecks = SOME_KOPECKS, .chips = SOME_CHIPS, .locker = {}};

  const Exchange refused = cashOutChips(purse, SOME_CHIPS + 1);

  EXPECT_EQ(refused.refusal, ExchangeRefusal::NotEnoughChips);
  EXPECT_EQ(refused.purse, purse);
}

/**
 * Given a purse of kopecks and chips
 * When it buys none, or cashes none out
 * Then the cashier refuses, nothing to exchange, both ways
 */
TEST(ChipsExchange, AnExchangeOfNothingIsRefused) {
  const Purse purse{.kopecks = SOME_KOPECKS, .chips = SOME_CHIPS, .locker = {}};

  EXPECT_EQ(buyChips(purse, 0).refusal, ExchangeRefusal::NothingToExchange);
  EXPECT_EQ(cashOutChips(purse, 0).refusal, ExchangeRefusal::NothingToExchange);
}

/**
 * Given a purse 500 chips short of the largest number of chips
 * When it buys 500 chips, then 501
 * Then the first fills the chips exactly, the second is refused as full
 */
TEST(ChipsExchange, BuyingStopsAtTheLargestNumberOfChips) {
  const Purse purse{.kopecks = SOME_KOPECKS,
                    .chips = AN_AMOUNT_SHORT_OF_THE_LARGEST,
                    .locker = {}};

  const Exchange filled = buyChips(purse, AN_AMOUNT);
  const Exchange refused = buyChips(purse, AN_AMOUNT + 1);

  EXPECT_EQ(filled.refusal, std::nullopt);
  EXPECT_EQ(filled.purse.chips, LARGEST_BALANCE);
  EXPECT_EQ(refused.refusal, ExchangeRefusal::BalanceFull);
  EXPECT_EQ(refused.purse, purse);
}

/**
 * Given a purse 500 kopecks short of the largest balance
 * When it cashes out 500 chips, then 501
 * Then the first fills the kopecks exactly, the second is refused as full
 */
TEST(ChipsExchange, CashingOutStopsAtTheLargestBalance) {
  const Purse purse{.kopecks = AN_AMOUNT_SHORT_OF_THE_LARGEST,
                    .chips = SOME_CHIPS,
                    .locker = {}};

  const Exchange filled = cashOutChips(purse, AN_AMOUNT);
  const Exchange refused = cashOutChips(purse, AN_AMOUNT + 1);

  EXPECT_EQ(filled.refusal, std::nullopt);
  EXPECT_EQ(filled.purse.kopecks, LARGEST_BALANCE);
  EXPECT_EQ(refused.refusal, ExchangeRefusal::BalanceFull);
  EXPECT_EQ(refused.purse, purse);
}
