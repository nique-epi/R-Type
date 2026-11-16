#include <gtest/gtest.h>
#include <optional>
#include "Currency.hpp"
#include "ExchangeRequest.hpp"
#include "Wallet.hpp"
#include "WalletBalances.hpp"
#include "WalletBalancesTestConstants.hpp"
#include "WalletExchangeRefusal.hpp"

using rtype::network::canReceive;
using rtype::network::creditedBalance;
using rtype::network::Currency;
using rtype::network::exchangedWallet;
using rtype::network::exchangeRefusalOf;
using rtype::network::ExchangeRequest;
using rtype::network::WalletExchangeRefusal;

/**
 * Given a wallet ten chips short of the largest balance
 * When it is asked whether it takes ten chips, then eleven
 * Then it takes ten and refuses eleven
 */
TEST(WalletBalances, AWalletTakesUpToItsLargestBalance) {
  EXPECT_TRUE(
      canReceive(TEN_CHIPS_SHORT_OF_THE_MAXIMUM, Currency::Chips, TEN_CHIPS));
  EXPECT_FALSE(canReceive(TEN_CHIPS_SHORT_OF_THE_MAXIMUM, Currency::Chips,
                          ELEVEN_CHIPS));
}

/**
 * Given a wallet ten chips short of the largest balance
 * When it is credited a hundred chips
 * Then its chips stop at the largest balance
 */
TEST(WalletBalances, ACreditStopsAtTheLargestBalance) {
  EXPECT_EQ(creditedBalance(TEN_CHIPS_SHORT_OF_THE_MAXIMUM, Currency::Chips,
                            A_HUNDRED_CHIPS),
            LARGEST_CHIPS);
}

/**
 * Given a wallet of 4 250 kopecks and 1 250 chips
 * When it buys 500 chips
 * Then the cashier takes 500 kopecks and gives 500 chips
 */
TEST(WalletBalances, BuyingChangesKopecksIntoAsManyChips) {
  const ExchangeRequest buying{.from = Currency::Kopecks, .amount = AN_AMOUNT};

  EXPECT_EQ(exchangeRefusalOf(A_WALLET, buying), std::nullopt);
  EXPECT_EQ(exchangedWallet(A_WALLET, buying), A_WALLET_AFTER_BUYING);
}

/**
 * Given a wallet of 4 250 kopecks and 1 250 chips
 * When it cashes out 500 chips
 * Then the cashier takes 500 chips and gives 500 kopecks
 */
TEST(WalletBalances, CashingOutChangesChipsIntoAsManyKopecks) {
  const ExchangeRequest cashing{.from = Currency::Chips, .amount = AN_AMOUNT};

  EXPECT_EQ(exchangeRefusalOf(A_WALLET, cashing), std::nullopt);
  EXPECT_EQ(exchangedWallet(A_WALLET, cashing), A_WALLET_AFTER_CASHING_OUT);
}

/**
 * Given a wallet of 4 250 kopecks and 1 250 chips
 * When it buys 500 chips, then cashes the same 500 out
 * Then it holds what it held: the cashier keeps no fee
 */
TEST(WalletBalances, ARoundTripKeepsTheWallet) {
  const rtype::network::Wallet bought = exchangedWallet(
      A_WALLET,
      ExchangeRequest{.from = Currency::Kopecks, .amount = AN_AMOUNT});
  const ExchangeRequest back{.from = Currency::Chips, .amount = AN_AMOUNT};

  EXPECT_EQ(exchangeRefusalOf(bought, back), std::nullopt);
  EXPECT_EQ(exchangedWallet(bought, back), A_WALLET);
}

/**
 * Given a wallet of 4 250 kopecks and no chip
 * When it buys exactly 4 250 chips
 * Then the cashier changes the whole balance and leaves no kopeck
 */
TEST(WalletBalances, BuyingTheWholeBalanceIsAccepted) {
  const ExchangeRequest buying{.from = Currency::Kopecks,
                               .amount = SOME_KOPECKS};

  EXPECT_EQ(exchangeRefusalOf(KOPECKS_ONLY, buying), std::nullopt);
  EXPECT_EQ(exchangedWallet(KOPECKS_ONLY, buying), ALL_KOPECKS_BOUGHT);
}

/**
 * Given a wallet of 4 250 kopecks
 * When it buys 4 251 chips
 * Then the cashier refuses, not enough kopecks
 */
TEST(WalletBalances, BuyingMoreThanTheKopecksIsRefused) {
  EXPECT_EQ(
      exchangeRefusalOf(A_WALLET, ExchangeRequest{.from = Currency::Kopecks,
                                                  .amount = SOME_KOPECKS + 1}),
      WalletExchangeRefusal::NotEnough);
}

/**
 * Given a wallet of no kopeck and 1 250 chips
 * When it cashes out exactly 1 250 chips
 * Then the cashier changes them all and leaves no chip
 */
TEST(WalletBalances, CashingOutEveryChipIsAccepted) {
  const ExchangeRequest cashing{.from = Currency::Chips, .amount = SOME_CHIPS};

  EXPECT_EQ(exchangeRefusalOf(CHIPS_ONLY, cashing), std::nullopt);
  EXPECT_EQ(exchangedWallet(CHIPS_ONLY, cashing), ALL_CHIPS_CASHED_OUT);
}

/**
 * Given a wallet of 1 250 chips
 * When it cashes out 1 251 chips
 * Then the cashier refuses, not enough chips
 */
TEST(WalletBalances, CashingOutMoreThanTheChipsIsRefused) {
  EXPECT_EQ(
      exchangeRefusalOf(A_WALLET, ExchangeRequest{.from = Currency::Chips,
                                                  .amount = SOME_CHIPS + 1}),
      WalletExchangeRefusal::NotEnough);
}

/**
 * Given a wallet of kopecks and chips
 * When it buys none, or cashes none out
 * Then the cashier refuses the amount, both ways
 */
TEST(WalletBalances, AnExchangeOfNothingIsRefused) {
  EXPECT_EQ(
      exchangeRefusalOf(
          A_WALLET, ExchangeRequest{.from = Currency::Kopecks, .amount = 0}),
      WalletExchangeRefusal::InvalidAmount);
  EXPECT_EQ(exchangeRefusalOf(A_WALLET, ExchangeRequest{.from = Currency::Chips,
                                                        .amount = 0}),
            WalletExchangeRefusal::InvalidAmount);
}

/**
 * Given a wallet 500 chips short of the largest number of chips
 * When it buys 500 chips, then 501
 * Then the first fills the chips exactly, the second is refused as full
 */
TEST(WalletBalances, BuyingStopsAtTheLargestNumberOfChips) {
  const ExchangeRequest filling{.from = Currency::Kopecks, .amount = AN_AMOUNT};

  EXPECT_EQ(exchangeRefusalOf(CHIPS_AN_AMOUNT_SHORT_OF_THE_LARGEST, filling),
            std::nullopt);
  EXPECT_EQ(
      exchangedWallet(CHIPS_AN_AMOUNT_SHORT_OF_THE_LARGEST, filling).chips,
      LARGEST_CHIPS);
  EXPECT_EQ(exchangeRefusalOf(CHIPS_AN_AMOUNT_SHORT_OF_THE_LARGEST,
                              ExchangeRequest{.from = Currency::Kopecks,
                                              .amount = AN_AMOUNT + 1}),
            WalletExchangeRefusal::PastTheMaximum);
}

/**
 * Given a wallet 500 kopecks short of the largest balance
 * When it cashes out 500 chips, then 501
 * Then the first fills the kopecks exactly, the second is refused as full
 */
TEST(WalletBalances, CashingOutStopsAtTheLargestBalance) {
  const ExchangeRequest filling{.from = Currency::Chips, .amount = AN_AMOUNT};

  EXPECT_EQ(exchangeRefusalOf(KOPECKS_AN_AMOUNT_SHORT_OF_THE_LARGEST, filling),
            std::nullopt);
  EXPECT_EQ(
      exchangedWallet(KOPECKS_AN_AMOUNT_SHORT_OF_THE_LARGEST, filling).kopecks,
      LARGEST_KOPECKS);
  EXPECT_EQ(exchangeRefusalOf(KOPECKS_AN_AMOUNT_SHORT_OF_THE_LARGEST,
                              ExchangeRequest{.from = Currency::Chips,
                                              .amount = AN_AMOUNT + 1}),
            WalletExchangeRefusal::PastTheMaximum);
}
