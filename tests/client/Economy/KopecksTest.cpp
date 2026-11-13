#include <gtest/gtest.h>
#include <cstdint>
#include "Kopecks.hpp"
#include "KopecksTestConstants.hpp"

using rtype::client::boundedKopecks;
using rtype::client::kopecksFor;
using rtype::client::kopecksText;
using rtype::client::withVictoryBonus;
using rtype::client::testing::DESIGN_SCORE;
using rtype::client::testing::FOUR_DIGIT_AMOUNT;
using rtype::client::testing::FOUR_DIGITS_WRITTEN;
using rtype::client::testing::KOPECKS_OF_THE_DESIGN_SCORE;
using rtype::client::testing::LARGEST_BALANCE;
using rtype::client::testing::LARGEST_BALANCE_WRITTEN;
using rtype::client::testing::LARGEST_SCORE;
using rtype::client::testing::ODD_GAIN;
using rtype::client::testing::ODD_GAIN_AFTER_A_VICTORY;
using rtype::client::testing::ROUND_GAIN;
using rtype::client::testing::ROUND_GAIN_AFTER_A_VICTORY;
using rtype::client::testing::SCORE_UNDER_ONE_KOPECK;
using rtype::client::testing::THREE_DIGIT_AMOUNT;
using rtype::client::testing::THREE_DIGITS_WRITTEN;

/**
 * Given a final score of 12450
 * When its kopecks are counted
 * Then it pays one kopeck per ten points: 1245
 */
TEST(Kopecks, TenPointsOfScorePayOneKopeck) {
  EXPECT_EQ(kopecksFor(DESIGN_SCORE), KOPECKS_OF_THE_DESIGN_SCORE);
}

/**
 * Given a final score under ten points
 * When its kopecks are counted
 * Then it pays none
 */
TEST(Kopecks, APartOfAKopeckIsLost) {
  EXPECT_EQ(kopecksFor(SCORE_UNDER_ONE_KOPECK), 0U);
}

/**
 * Given 1000 kopecks earned by a score
 * When the squadron won
 * Then the game pays a quarter more: 1250
 */
TEST(Kopecks, AVictoryPaysAQuarterMore) {
  EXPECT_EQ(withVictoryBonus(ROUND_GAIN), ROUND_GAIN_AFTER_A_VICTORY);
}

/**
 * Given 30 kopecks, whose quarter is 7.5
 * When the squadron won
 * Then the bonus is rounded down: 37
 */
TEST(Kopecks, TheBonusOfAVictoryIsRoundedDown) {
  EXPECT_EQ(withVictoryBonus(ODD_GAIN), ODD_GAIN_AFTER_A_VICTORY);
}

/**
 * Given the largest score and the largest balance
 * When they are paid, raised by a victory, or passed by one kopeck
 * Then no amount passes the largest balance
 */
TEST(Kopecks, NoAmountPassesTheLargestBalance) {
  EXPECT_EQ(kopecksFor(LARGEST_SCORE), LARGEST_BALANCE);
  EXPECT_EQ(withVictoryBonus(LARGEST_BALANCE), LARGEST_BALANCE);
  EXPECT_EQ(boundedKopecks(std::uint64_t{LARGEST_BALANCE} + 1),
            LARGEST_BALANCE);
}

/**
 * Given amounts of three, four and six digits
 * When they are written
 * Then a space parts their thousands, and none starts a short amount
 */
TEST(Kopecks, ASpacePartsTheThousands) {
  EXPECT_EQ(kopecksText(THREE_DIGIT_AMOUNT), THREE_DIGITS_WRITTEN);
  EXPECT_EQ(kopecksText(FOUR_DIGIT_AMOUNT), FOUR_DIGITS_WRITTEN);
  EXPECT_EQ(kopecksText(LARGEST_BALANCE), LARGEST_BALANCE_WRITTEN);
}
