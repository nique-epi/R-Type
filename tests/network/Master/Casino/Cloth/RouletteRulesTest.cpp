#include <gtest/gtest.h>
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteRules.hpp"
#include "RouletteRulesFixtures.hpp"
#include "RouletteRulesTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given a bet of ten chips and a number drawn
 * When its payout is worked out
 * Then it is the return of its kind when it covers the number, 0 otherwise
 */
TEST_P(RouletteRulesPayout, PaysTheReturnOfItsKindOnACoveredNumber) {
  const PayoutCase& payoutCase = GetParam();

  EXPECT_EQ(payoutOf(payoutCase.bet, payoutCase.drawn), payoutCase.payout);
}

INSTANTIATE_TEST_SUITE_P(EveryKind, RouletteRulesPayout,
                         ::testing::ValuesIn(PAYOUT_CASES));

/**
 * Given a bet that does not lie on the cloth
 * When it is checked, and paid on the number it names
 * Then it is not placeable, and pays nothing
 */
TEST_P(RouletteRulesRefused, RefusesABetOffTheCloth) {
  const RouletteBet& bet = GetParam();

  EXPECT_FALSE(isPlaceableBet(bet));
  EXPECT_EQ(payoutOf(bet, bet.firstNumber), 0U);
}

INSTANTIATE_TEST_SUITE_P(EveryKind, RouletteRulesRefused,
                         ::testing::ValuesIn(REFUSED_BETS));

/**
 * Given the last bet of each kind on the cloth
 * When it is checked
 * Then it is placeable
 */
TEST_P(RouletteRulesPlaced, PlacesTheLastBetOfEachKind) {
  EXPECT_TRUE(isPlaceableBet(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(EveryKind, RouletteRulesPlaced,
                         ::testing::ValuesIn(PLACED_BETS));

/**
 * Given a straight bet on 0 and a number past the wheel
 * When its payout is worked out
 * Then it is 0
 */
TEST(RouletteRules, PaysNothingOnANumberPastTheWheel) {
  EXPECT_EQ(payoutOf(RouletteBet{.kind = RouletteBetKind::Straight,
                                 .firstNumber = 0,
                                 .amount = TEN_CHIPS},
                     NUMBER_PAST_THE_WHEEL),
            0U);
}

}  // namespace rtype::network::testing
