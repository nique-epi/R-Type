#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>
#include "FinalPilot.hpp"
#include "FinalRanking.hpp"
#include "FinalRankingFixtures.hpp"
#include "FinalRankingTestConstants.hpp"

using rtype::client::FinalPilot;
using rtype::client::finalPilotsOf;
using rtype::client::rankedPilots;
using rtype::client::teamScoreOf;
using rtype::client::testing::FIRST_NICKNAME;
using rtype::client::testing::FIRST_NUMBER;
using rtype::client::testing::FIRST_RANK;
using rtype::client::testing::HIGH_SCORE;
using rtype::client::testing::LARGEST_UNSIGNED;
using rtype::client::testing::LOW_PLUS_HIGH_PLUS_MIDDLE;
using rtype::client::testing::LOW_SCORE;
using rtype::client::testing::MIDDLE_SCORE;
using rtype::client::testing::SECOND_NICKNAME;
using rtype::client::testing::SECOND_NUMBER;
using rtype::client::testing::THIRD_NICKNAME;
using rtype::client::testing::THIRD_NUMBER;
using rtype::client::testing::THIRD_RANK;
using rtype::client::testing::UNLISTED_NUMBER;
using ::testing::ElementsAre;
using ::testing::Field;

/**
 * Given three pilots who scored 100, 300 and 200
 * When the game ends
 * Then they are ranked 300, 200, 100
 */
TEST(FinalRanking, TheBestScoreRanksFirst) {
  const std::vector<FinalPilot> ranked = rankedPilots(
      finalPilotsOf(boardOfThree(), endedWith({{FIRST_NUMBER, LOW_SCORE},
                                               {SECOND_NUMBER, HIGH_SCORE},
                                               {THIRD_NUMBER, MIDDLE_SCORE}})));

  EXPECT_THAT(ranked,
              ElementsAre(Field(&FinalPilot::nickname, SECOND_NICKNAME),
                          Field(&FinalPilot::nickname, THIRD_NICKNAME),
                          Field(&FinalPilot::nickname, FIRST_NICKNAME)));
}

/**
 * Given two pilots with the same best score, numbers 3 then 1
 * When the game ends
 * Then both rank first, number 1 listed before number 3
 */
TEST(FinalRanking, EqualScoresShareARankInNumberOrder) {
  const std::vector<FinalPilot> ranked = rankedPilots(finalPilotsOf(
      boardOfThree(),
      endedWith({{THIRD_NUMBER, HIGH_SCORE}, {FIRST_NUMBER, HIGH_SCORE}})));

  EXPECT_THAT(ranked,
              ElementsAre(Field(&FinalPilot::playerNumber, FIRST_NUMBER),
                          Field(&FinalPilot::playerNumber, THIRD_NUMBER)));
  EXPECT_EQ(ranked[1].rank, FIRST_RANK);
}

/**
 * Given two pilots tied first and a third below them
 * When the game ends
 * Then the third ranks third, the second rank being skipped
 */
TEST(FinalRanking, TheRankAfterATieSkips) {
  const std::vector<FinalPilot> ranked = rankedPilots(
      finalPilotsOf(boardOfThree(), endedWith({{FIRST_NUMBER, HIGH_SCORE},
                                               {SECOND_NUMBER, HIGH_SCORE},
                                               {THIRD_NUMBER, LOW_SCORE}})));

  EXPECT_EQ(ranked.back().rank, THIRD_RANK);
}

/**
 * Given the board where NOVA, number 2, is this client
 * When the game ends
 * Then number 2 is named NOVA and marked as this client
 */
TEST(FinalRanking, NicknamesAndThisClientComeFromTheBoard) {
  const std::vector<FinalPilot> pilots =
      finalPilotsOf(boardOfThree(), endedWith({{SECOND_NUMBER, LOW_SCORE}}));

  ASSERT_EQ(pilots.size(), 1U);
  EXPECT_EQ(pilots[0].nickname, SECOND_NICKNAME);
  EXPECT_TRUE(pilots[0].isLocalPlayer);
}

/**
 * Given a player the board does not list
 * When the game ends with its score
 * Then it has no nickname and is not this client
 */
TEST(FinalRanking, APlayerTheBoardDidNotListHasNoNickname) {
  const std::vector<FinalPilot> pilots =
      finalPilotsOf(boardOfThree(), endedWith({{UNLISTED_NUMBER, LOW_SCORE}}));

  ASSERT_EQ(pilots.size(), 1U);
  EXPECT_TRUE(pilots[0].nickname.empty());
  EXPECT_FALSE(pilots[0].isLocalPlayer);
}

/**
 * Given three pilots who scored 100, 300 and 200
 * When the team score is asked
 * Then it is 600
 */
TEST(FinalRanking, TheTeamScoreAddsEveryScore) {
  const std::vector<FinalPilot> pilots =
      finalPilotsOf(boardOfThree(), endedWith({{FIRST_NUMBER, LOW_SCORE},
                                               {SECOND_NUMBER, HIGH_SCORE},
                                               {THIRD_NUMBER, MIDDLE_SCORE}}));

  EXPECT_EQ(teamScoreOf(pilots), LOW_PLUS_HIGH_PLUS_MIDDLE);
}

/**
 * Given two pilots who both reached the largest score a message carries
 * When the team score is asked
 * Then it stops at that largest score instead of wrapping around
 */
TEST(FinalRanking, TheTeamScoreNeverWrapsAround) {
  const std::vector<FinalPilot> pilots = finalPilotsOf(
      boardOfThree(), endedWith({{FIRST_NUMBER, LARGEST_UNSIGNED},
                                 {SECOND_NUMBER, LARGEST_UNSIGNED}}));

  EXPECT_EQ(teamScoreOf(pilots), LARGEST_UNSIGNED);
}
