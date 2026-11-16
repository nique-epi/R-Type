#include <gtest/gtest.h>
#include "Currency.hpp"
#include "WalletBalances.hpp"
#include "WalletBalancesTestConstants.hpp"

using rtype::network::canReceive;
using rtype::network::creditedBalance;
using rtype::network::Currency;

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
