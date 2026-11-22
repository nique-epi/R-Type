#include <gtest/gtest.h>
#include <array>
#include "FinalPilot.hpp"
#include "KopecksReward.hpp"
#include "KopecksRewardTestConstants.hpp"

using rtype::client::FinalPilot;
using rtype::client::kopecksEarnedBy;
using rtype::client::testing::ANOTHER_PILOT_BOUNTY;
using rtype::client::testing::ANOTHER_PILOT_SCORE;
using rtype::client::testing::OWN_BOUNTY;
using rtype::client::testing::OWN_FINAL_SCORE;
using rtype::client::testing::OWN_GAIN;

/**
 * Given two final pilots, the second being this client with 12450 points and
 * a bounty of 650
 * When the kopecks of the game are counted
 * Then they are those of its own score and bounty: 899, whatever the other
 * earned
 */
TEST(KopecksReward, ThisClientIsPaidItsScoreAndItsBounty) {
  const std::array pilots{FinalPilot{.playerNumber = 1,
                                     .nickname = {},
                                     .score = ANOTHER_PILOT_SCORE,
                                     .bounty = ANOTHER_PILOT_BOUNTY},
                          FinalPilot{.playerNumber = 2,
                                     .nickname = {},
                                     .score = OWN_FINAL_SCORE,
                                     .bounty = OWN_BOUNTY,
                                     .isLocalPlayer = true}};

  EXPECT_EQ(kopecksEarnedBy(pilots), OWN_GAIN);
}

/**
 * Given final pilots none of whom is this client
 * When the kopecks of the game are counted
 * Then there are none
 */
TEST(KopecksReward, AGameWithoutThisClientPaysNothing) {
  const std::array pilots{FinalPilot{.playerNumber = 1,
                                     .nickname = {},
                                     .score = ANOTHER_PILOT_SCORE,
                                     .bounty = ANOTHER_PILOT_BOUNTY}};

  EXPECT_EQ(kopecksEarnedBy(pilots), 0U);
}
