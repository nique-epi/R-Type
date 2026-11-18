#include <gtest/gtest.h>
#include <array>
#include "FinalPilot.hpp"
#include "GameEnded.hpp"
#include "KopecksReward.hpp"
#include "KopecksRewardTestConstants.hpp"

using rtype::client::FinalPilot;
using rtype::client::kopecksEarnedBy;
using rtype::client::testing::ANOTHER_PILOT_SCORE;
using rtype::client::testing::OWN_FINAL_SCORE;
using rtype::client::testing::OWN_GAIN_AFTER_A_DEFEAT;
using rtype::client::testing::OWN_GAIN_AFTER_A_VICTORY;
using rtype::network::WireGameOutcome;

/**
 * Given two final pilots, the second being this client with 12450 points
 * When the kopecks of a lost game are counted
 * Then they are those of its own score: 1245, whatever the other scored
 */
TEST(KopecksReward, ADefeatPaysTheScoreOfThisClient) {
  const std::array pilots{
      FinalPilot{
          .playerNumber = 1, .nickname = {}, .score = ANOTHER_PILOT_SCORE},
      FinalPilot{.playerNumber = 2,
                 .nickname = {},
                 .score = OWN_FINAL_SCORE,
                 .isLocalPlayer = true}};

  EXPECT_EQ(kopecksEarnedBy(pilots, WireGameOutcome::Defeat),
            OWN_GAIN_AFTER_A_DEFEAT);
}

/**
 * Given the same pilots
 * When the kopecks of a won game are counted
 * Then they are a quarter more: 1556
 */
TEST(KopecksReward, AVictoryPaysAQuarterMore) {
  const std::array pilots{
      FinalPilot{
          .playerNumber = 1, .nickname = {}, .score = ANOTHER_PILOT_SCORE},
      FinalPilot{.playerNumber = 2,
                 .nickname = {},
                 .score = OWN_FINAL_SCORE,
                 .isLocalPlayer = true}};

  EXPECT_EQ(kopecksEarnedBy(pilots, WireGameOutcome::Victory),
            OWN_GAIN_AFTER_A_VICTORY);
}

/**
 * Given final pilots none of whom is this client
 * When the kopecks of the game are counted
 * Then there are none
 */
TEST(KopecksReward, AGameWithoutThisClientPaysNothing) {
  const std::array pilots{FinalPilot{
      .playerNumber = 1, .nickname = {}, .score = ANOTHER_PILOT_SCORE}};

  EXPECT_EQ(kopecksEarnedBy(pilots, WireGameOutcome::Victory), 0U);
}
