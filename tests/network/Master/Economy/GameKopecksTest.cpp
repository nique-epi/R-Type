#include <gtest/gtest.h>
#include <cstdint>
#include "GameKopecks.hpp"
#include "GameKopecksTestConstants.hpp"

using rtype::network::boundedKopecks;
using rtype::network::kopecksFor;
using rtype::network::withVictoryBonus;
using rtype::network::testing::DESIGN_SCORE;
using rtype::network::testing::KOPECKS_OF_THE_DESIGN_SCORE;
using rtype::network::testing::LARGEST_BALANCE;
using rtype::network::testing::LARGEST_SCORE;
using rtype::network::testing::ODD_GAIN;
using rtype::network::testing::ODD_GAIN_AFTER_A_VICTORY;
using rtype::network::testing::ROUND_GAIN;
using rtype::network::testing::ROUND_GAIN_AFTER_A_VICTORY;
using rtype::network::testing::SCORE_UNDER_ONE_KOPECK;

/**
 * Given a final score of 12450
 * When its kopecks are counted
 * Then it pays one kopeck per ten points: 1245
 */
TEST(GameKopecks, TenPointsOfScorePayOneKopeck) {
  EXPECT_EQ(kopecksFor(DESIGN_SCORE), KOPECKS_OF_THE_DESIGN_SCORE);
}

/**
 * Given a final score under ten points
 * When its kopecks are counted
 * Then it pays none
 */
TEST(GameKopecks, APartOfAKopeckIsLost) {
  EXPECT_EQ(kopecksFor(SCORE_UNDER_ONE_KOPECK), 0U);
}

/**
 * Given 1000 kopecks earned by a score
 * When the squadron won
 * Then the game pays a quarter more: 1250
 */
TEST(GameKopecks, AVictoryPaysAQuarterMore) {
  EXPECT_EQ(withVictoryBonus(ROUND_GAIN), ROUND_GAIN_AFTER_A_VICTORY);
}

/**
 * Given 30 kopecks, whose quarter is 7.5
 * When the squadron won
 * Then the bonus is rounded down: 37
 */
TEST(GameKopecks, TheBonusOfAVictoryIsRoundedDown) {
  EXPECT_EQ(withVictoryBonus(ODD_GAIN), ODD_GAIN_AFTER_A_VICTORY);
}

/**
 * Given the largest score and the largest balance
 * When they are paid, raised by a victory, or passed by one kopeck
 * Then no amount passes the largest balance
 */
TEST(GameKopecks, NoAmountPassesTheLargestBalance) {
  EXPECT_EQ(kopecksFor(LARGEST_SCORE), LARGEST_BALANCE);
  EXPECT_EQ(withVictoryBonus(LARGEST_BALANCE), LARGEST_BALANCE);
  EXPECT_EQ(boundedKopecks(std::uint64_t{LARGEST_BALANCE} + 1),
            LARGEST_BALANCE);
}
