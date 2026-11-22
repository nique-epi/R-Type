#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteSlip.hpp"
#include "RouletteSlipTestConstants.hpp"

using rtype::client::RouletteSlip;
using rtype::client::testing::CHIPS_FOR_ONE_CHIP;
using rtype::client::testing::CHIPS_OF_500_IN_THE_CAP;
using rtype::client::testing::FIFTY_ON_SEVENTEEN;
using rtype::client::testing::FIVE_HUNDRED_ON_EIGHTEEN;
using rtype::client::testing::FIVE_HUNDRED_ON_RED;
using rtype::client::testing::FIVE_HUNDRED_ON_SEVENTEEN;
using rtype::client::testing::LAST_NUMBER_OF_THE_STACKS;
using rtype::client::testing::MOST_STACKS;
using rtype::client::testing::ONE_CHIP_OF_500;
using rtype::client::testing::ONE_ON_SEVENTEEN;
using rtype::client::testing::ONE_ON_THIRTY_THREE;
using rtype::client::testing::PLENTY_OF_CHIPS;
using rtype::client::testing::ROUND_CAP;
using rtype::client::testing::TWENTY_FIVE_ON_RED;
using rtype::client::testing::TWENTY_FIVE_ON_SEVENTEEN;
using rtype::client::testing::TWO_CHIPS_OF_500;
using rtype::network::RouletteBet;
using rtype::network::RouletteBetKind;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given an empty slip
 * When a chip of 25 is laid on 17 twice
 * Then 17 holds one stack of 50
 */
TEST(RouletteSlip, AChipOnAStackAddsToIt) {
  RouletteSlip slip;

  EXPECT_TRUE(slip.lay(TWENTY_FIVE_ON_SEVENTEEN, PLENTY_OF_CHIPS));
  EXPECT_TRUE(slip.lay(TWENTY_FIVE_ON_SEVENTEEN, PLENTY_OF_CHIPS));

  EXPECT_THAT(slip.bets(), ElementsAre(FIFTY_ON_SEVENTEEN));
}

/**
 * Given a player holding chips for one chip of 25 only
 * When a chip of 25 is laid, then another
 * Then the second is refused, the first kept
 */
TEST(RouletteSlip, NoChipPastTheChipsHeld) {
  RouletteSlip slip;
  ASSERT_TRUE(slip.lay(TWENTY_FIVE_ON_SEVENTEEN, CHIPS_FOR_ONE_CHIP));

  EXPECT_FALSE(slip.lay(TWENTY_FIVE_ON_RED, CHIPS_FOR_ONE_CHIP));

  EXPECT_THAT(slip.bets(), ElementsAre(TWENTY_FIVE_ON_SEVENTEEN));
}

/**
 * Given a slip holding the 2 000 chips a round takes at most
 * When one more chip is laid, the player holding plenty
 * Then it is refused and the stake stays 2 000
 */
TEST(RouletteSlip, NoChipPastTheCapOfARound) {
  RouletteSlip slip;
  for (std::size_t chip = 0; chip < CHIPS_OF_500_IN_THE_CAP; ++chip) {
    ASSERT_TRUE(slip.lay(FIVE_HUNDRED_ON_RED, PLENTY_OF_CHIPS));
  }

  EXPECT_FALSE(slip.lay(TWENTY_FIVE_ON_SEVENTEEN, PLENTY_OF_CHIPS));

  EXPECT_EQ(slip.stake(), ROUND_CAP);
}

/**
 * Given a slip holding 500 on 17, which returns the 18 000 a round returns at
 * most
 * When a chip of 1 more is laid on 17, the player holding plenty
 * Then it is refused and the stake stays 500
 */
TEST(RouletteSlip, NoChipPastTheMostARoundReturns) {
  RouletteSlip slip;
  ASSERT_TRUE(slip.lay(FIVE_HUNDRED_ON_SEVENTEEN, PLENTY_OF_CHIPS));

  EXPECT_FALSE(slip.lay(ONE_ON_SEVENTEEN, PLENTY_OF_CHIPS));

  EXPECT_EQ(slip.stake(), ONE_CHIP_OF_500);
}

/**
 * Given a slip holding 500 on 17
 * When 500 is laid on 18, which 17 does not cover
 * Then it is laid: whatever number comes out, the round returns 18 000 at
 * most
 */
TEST(RouletteSlip, TwoNumbersEachReturnTheMostARoundReturns) {
  RouletteSlip slip;
  ASSERT_TRUE(slip.lay(FIVE_HUNDRED_ON_SEVENTEEN, PLENTY_OF_CHIPS));

  EXPECT_TRUE(slip.lay(FIVE_HUNDRED_ON_EIGHTEEN, PLENTY_OF_CHIPS));

  EXPECT_EQ(slip.stake(), TWO_CHIPS_OF_500);
}

/**
 * Given a slip holding 32 stacks, the most a round takes
 * When a chip is laid on a 33rd place
 * Then it is refused
 */
TEST(RouletteSlip, NoStackPastTheMostARoundTakes) {
  RouletteSlip slip;
  for (std::uint8_t number = 1; number <= LAST_NUMBER_OF_THE_STACKS; ++number) {
    ASSERT_TRUE(slip.lay(RouletteBet{.kind = RouletteBetKind::Straight,
                                     .firstNumber = number,
                                     .amount = 1},
                         PLENTY_OF_CHIPS));
  }

  EXPECT_FALSE(slip.lay(ONE_ON_THIRTY_THREE, PLENTY_OF_CHIPS));

  EXPECT_EQ(slip.bets().size(), MOST_STACKS);
}

/**
 * Given a slip holding bets
 * When it is cleared
 * Then it holds none
 */
TEST(RouletteSlip, ClearTakesEveryStackOff) {
  RouletteSlip slip;
  ASSERT_TRUE(slip.lay(TWENTY_FIVE_ON_SEVENTEEN, PLENTY_OF_CHIPS));

  slip.clear();

  EXPECT_THAT(slip.bets(), IsEmpty());
}

/**
 * Given a slip holding a chip on 17
 * When it is replaced by bets past the chips held
 * Then they are refused and the slip keeps its chip
 */
TEST(RouletteSlip, BetsPastTheChipsHeldDoNotReplaceTheSlip) {
  RouletteSlip slip;
  ASSERT_TRUE(slip.lay(TWENTY_FIVE_ON_SEVENTEEN, PLENTY_OF_CHIPS));

  EXPECT_FALSE(slip.replaceWith({FIVE_HUNDRED_ON_RED}, CHIPS_FOR_ONE_CHIP));

  EXPECT_THAT(slip.bets(), ElementsAre(TWENTY_FIVE_ON_SEVENTEEN));
}
