#include <gtest/gtest.h>
#include <cstdint>
#include "GameGain.hpp"
#include "GameKopecks.hpp"
#include "GameKopecksTestConstants.hpp"

using rtype::network::boundedKopecks;
using rtype::network::GameGain;
using rtype::network::kopecksFor;
using rtype::network::testing::BOUNTY_OF_THE_PIT;
using rtype::network::testing::DESIGN_SCORE;
using rtype::network::testing::KOPECKS_OF_THE_DESIGN_SCORE;
using rtype::network::testing::KOPECKS_OF_THE_PIT;
using rtype::network::testing::LARGEST_BALANCE;
using rtype::network::testing::LARGEST_BOUNTY;
using rtype::network::testing::LARGEST_SCORE;
using rtype::network::testing::SCORE_UNDER_ONE_KOPECK;

/**
 * Given a final score of 12450 and no bounty
 * When its kopecks are counted
 * Then it pays one kopeck per fifty points: 249
 */
TEST(GameKopecks, FiftyPointsOfScorePayOneKopeck) {
  EXPECT_EQ(kopecksFor(GameGain{.score = DESIGN_SCORE, .bounty = 0}),
            KOPECKS_OF_THE_DESIGN_SCORE);
}

/**
 * Given a final score under fifty points
 * When its kopecks are counted
 * Then it pays none
 */
TEST(GameKopecks, APartOfAKopeckIsLost) {
  EXPECT_EQ(kopecksFor(GameGain{.score = SCORE_UNDER_ONE_KOPECK, .bounty = 0}),
            0U);
}

/**
 * Given a final score of 12450 and the bounty of the Pit, 650
 * When its kopecks are counted
 * Then it pays the bounty and the kopecks of the score: 899
 */
TEST(GameKopecks, TheBountyIsPaidWithTheScore) {
  EXPECT_EQ(
      kopecksFor(GameGain{.score = DESIGN_SCORE, .bounty = BOUNTY_OF_THE_PIT}),
      KOPECKS_OF_THE_PIT);
}

/**
 * Given the largest score, the largest bounty and the largest balance
 * When they are paid, or passed by one kopeck
 * Then no amount passes the largest balance
 */
TEST(GameKopecks, NoAmountPassesTheLargestBalance) {
  EXPECT_EQ(kopecksFor(GameGain{.score = LARGEST_SCORE, .bounty = 0}),
            LARGEST_BALANCE);
  EXPECT_EQ(kopecksFor(GameGain{.score = 0, .bounty = LARGEST_BOUNTY}),
            LARGEST_BALANCE);
  EXPECT_EQ(boundedKopecks(std::uint64_t{LARGEST_BALANCE} + 1),
            LARGEST_BALANCE);
}
