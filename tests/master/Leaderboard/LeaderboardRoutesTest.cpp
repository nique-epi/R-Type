#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include "FinishedGame.hpp"
#include "FinishedGamePlayer.hpp"
#include "LeaderboardRoutesFixture.hpp"
#include "LeaderboardRoutesFixtures.hpp"
#include "LeaderboardRoutesTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"

using rtype::network::FinishedGame;
using rtype::network::FinishedGamePlayer;
using rtype::network::problemDetailOf;
using rtype::network::testing::FIRST_PILOT;
using rtype::network::testing::FIRST_PILOT_SCORE;
using rtype::network::testing::FIRST_RANK;
using rtype::network::testing::PLAYERS_POINTER;
using rtype::network::testing::quoted;
using rtype::network::testing::SECOND_PILOT;
using rtype::network::testing::SECOND_PILOT_ACCOUNT;
using rtype::network::testing::SECOND_PILOT_SCORE;
using rtype::network::testing::THIRD_PILOT;
using ::testing::ElementsAre;
using ::testing::HasSubstr;
using ::testing::IsEmpty;

/**
 * Given a server that sent the result of a game
 * When it sends the same result again
 * Then the master answers as a success
 */
TEST_F(LeaderboardRoutesFixture, AResultSentAgainIsAnsweredAsASuccess) {
  const FinishedGame game =
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE),
                       guestScoring(SECOND_PILOT, SECOND_PILOT_SCORE)});
  static_cast<void>(sendResult(paris(), FIRST_GAME_ID, game));

  const httplib::Result again = sendResult(paris(), FIRST_GAME_ID, game);

  ASSERT_TRUE(again);
  EXPECT_EQ(again->status, EXPECTED_NO_CONTENT);
}

/**
 * Given a server that sent the result of a game of two players
 * When it sends the same result again
 * Then the leaderboard still lists each player once
 */
TEST_F(LeaderboardRoutesFixture, AResultSentAgainAddsNoRow) {
  const FinishedGame game =
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE),
                       guestScoring(SECOND_PILOT, SECOND_PILOT_SCORE)});
  static_cast<void>(sendResult(paris(), FIRST_GAME_ID, game));

  static_cast<void>(sendResult(paris(), FIRST_GAME_ID, game));

  EXPECT_THAT(membersIn<std::string>(leaderboardOf(NORMAL_FOR_TWO_QUERY),
                                     NICKNAME_MEMBER_NAME),
              ElementsAre(FIRST_PILOT, SECOND_PILOT));
}

/**
 * Given a server
 * When it sends the result of a game that was not vanilla
 * Then the master answers 400, whose detail says the game was not vanilla
 */
TEST_F(LeaderboardRoutesFixture, AGameThatWasNotVanillaIsRefused) {
  FinishedGame game =
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE)});
  game.vanilla = false;

  const httplib::Result answer = sendResult(paris(), FIRST_GAME_ID, game);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
  EXPECT_THAT(problemDetailOf(answer->body),
              HasSubstr(NOT_VANILLA_DETAIL_PART));
}

/**
 * Given a server whose game that was not vanilla was refused
 * When the leaderboard of that game is read
 * Then it lists no score
 */
TEST_F(LeaderboardRoutesFixture, AGameThatWasNotVanillaReachesNoLeaderboard) {
  FinishedGame game =
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE)});
  game.vanilla = false;

  static_cast<void>(sendResult(paris(), FIRST_GAME_ID, game));

  EXPECT_THAT(membersIn<std::string>(leaderboardOf(NORMAL_FOR_TWO_QUERY),
                                     NICKNAME_MEMBER_NAME),
              IsEmpty());
}

/**
 * Given a master
 * When a result is sent without a key, then with a key no server has
 * Then each is answered 401
 */
TEST_F(LeaderboardRoutesFixture, AResultWithoutAValidKeyIsRefused) {
  const std::string path =
      std::string{OWN_RESULT_URL_PREFIX} + std::string{FIRST_GAME_ID};
  const std::string body = rtype::network::finishedGameText(
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE)}));
  for (const std::string_view key : {std::string_view{}, UNKNOWN_SERVER_KEY}) {
    SCOPED_TRACE(key);
    const httplib::Result answer = send(RouteCall{
        .method = PUT_METHOD, .path = path, .key = key, .body = body});

    ASSERT_TRUE(answer);
    EXPECT_EQ(answer->status, EXPECTED_UNAUTHORIZED);
  }
}

/**
 * Given a server
 * When it sends a result under a path that names no id a server draws
 * Then the master answers 400
 */
TEST_F(LeaderboardRoutesFixture, AnIdThatIsNoGameIdIsRefused) {
  const httplib::Result answer = sendResult(
      paris(), MALFORMED_GAME_ID,
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE)}));

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
}

/**
 * Given a server
 * When it sends the result of a game that names no player
 * Then the master answers 400, whose detail names the players
 */
TEST_F(LeaderboardRoutesFixture, AResultOutOfItsBoundsNamesItsMember) {
  const httplib::Result answer =
      sendResult(paris(), FIRST_GAME_ID, normalGameOfTwo({}));

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
  EXPECT_THAT(problemDetailOf(answer->body),
              HasSubstr(quoted(PLAYERS_POINTER)));
}

/**
 * Given FIRST_PILOT in a game of "normal" and THIRD_PILOT in a game of "hard",
 * both for two When the leaderboard of "hard" is read Then it lists THIRD_PILOT
 * alone
 */
TEST_F(LeaderboardRoutesFixture, TwoDifficultiesNeverMix) {
  static_cast<void>(sendResult(
      paris(), FIRST_GAME_ID,
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE)})));
  FinishedGame hard =
      normalGameOfTwo({guestScoring(THIRD_PILOT, SECOND_PILOT_SCORE)});
  hard.difficulty = std::string{HARD_DIFFICULTY};
  static_cast<void>(sendResult(paris(), SECOND_GAME_ID, hard));

  EXPECT_THAT(membersIn<std::string>(leaderboardOf(HARD_FOR_TWO_QUERY),
                                     NICKNAME_MEMBER_NAME),
              ElementsAre(THIRD_PILOT));
}

/**
 * Given FIRST_PILOT in a game for two and THIRD_PILOT in a game for one, both
 * of "normal" When the leaderboard of "normal" for one player is read Then it
 * lists THIRD_PILOT alone
 */
TEST_F(LeaderboardRoutesFixture, TwoNumbersOfPlayersNeverMix) {
  static_cast<void>(sendResult(
      paris(), FIRST_GAME_ID,
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE)})));
  FinishedGame alone =
      normalGameOfTwo({guestScoring(THIRD_PILOT, SECOND_PILOT_SCORE)});
  alone.playerCount = ONE_PLAYER;
  static_cast<void>(sendResult(paris(), SECOND_GAME_ID, alone));

  EXPECT_THAT(membersIn<std::string>(leaderboardOf(NORMAL_FOR_ONE_QUERY),
                                     NICKNAME_MEMBER_NAME),
              ElementsAre(THIRD_PILOT));
}

/**
 * Given FIRST_PILOT on "Paris 1" and THIRD_PILOT on "Lyon 1", in the same
 * leaderboard When the leaderboard is read for every server Then it lists both,
 * each row naming its server
 */
TEST_F(LeaderboardRoutesFixture, TheGlobalLeaderboardGathersEveryServer) {
  static_cast<void>(sendResult(
      paris(), FIRST_GAME_ID,
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE)})));
  static_cast<void>(sendResult(
      lyon(), FIRST_GAME_ID,
      normalGameOfTwo({guestScoring(THIRD_PILOT, SECOND_PILOT_SCORE)})));

  EXPECT_THAT(membersIn<std::string>(leaderboardOf(NORMAL_FOR_TWO_QUERY),
                                     SERVER_NAME_MEMBER_NAME),
              ElementsAre(TEST_SERVER_NAME, LYON_SERVER_NAME));
}

/**
 * Given FIRST_PILOT on "Paris 1" and THIRD_PILOT on "Lyon 1", in the same
 * leaderboard When the leaderboard of "Lyon 1" is read Then it lists
 * THIRD_PILOT alone
 */
TEST_F(LeaderboardRoutesFixture, TheLeaderboardOfOneServerHoldsItsRowsAlone) {
  static_cast<void>(sendResult(
      paris(), FIRST_GAME_ID,
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE)})));
  static_cast<void>(sendResult(
      lyon(), FIRST_GAME_ID,
      normalGameOfTwo({guestScoring(THIRD_PILOT, SECOND_PILOT_SCORE)})));

  const std::string query = std::string{NORMAL_FOR_TWO_QUERY} +
                            std::string{SERVER_QUERY_START} +
                            std::to_string(lyon().id);

  EXPECT_THAT(
      membersIn<std::string>(leaderboardOf(query), NICKNAME_MEMBER_NAME),
      ElementsAre(THIRD_PILOT));
}

/**
 * Given FIRST_PILOT in two games, scoring low then high
 * When the leaderboard is read
 * Then FIRST_PILOT has one row, of its high score
 */
TEST_F(LeaderboardRoutesFixture, APlayerKeepsItsBestScoreOnly) {
  static_cast<void>(sendResult(
      paris(), FIRST_GAME_ID,
      normalGameOfTwo({guestScoring(FIRST_PILOT, SECOND_PILOT_SCORE)})));
  static_cast<void>(sendResult(
      lyon(), FIRST_GAME_ID,
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE)})));

  const nlohmann::json leaderboard = leaderboardOf(NORMAL_FOR_TWO_QUERY);

  ASSERT_THAT(membersIn<std::string>(leaderboard, NICKNAME_MEMBER_NAME),
              ElementsAre(FIRST_PILOT));
  EXPECT_EQ(leaderboard.at(std::string{SCORES_MEMBER_NAME})
                .at(0)
                .at(std::string{SCORE_MEMBER_NAME})
                .get<std::uint32_t>(),
            FIRST_PILOT_SCORE);
}

/**
 * Given FIRST_PILOT playing under an account with one nickname, then another
 * When the leaderboard is read
 * Then the account has one row, of the game of its best score
 */
TEST_F(LeaderboardRoutesFixture, AnAccountIsOnePlayerUnderAnyNickname) {
  FinishedGamePlayer first = guestScoring(SECOND_PILOT, SECOND_PILOT_SCORE);
  first.accountId = std::string{SECOND_PILOT_ACCOUNT};
  FinishedGamePlayer second =
      guestScoring(OTHER_NICKNAME_OF_SECOND_PILOT, FIRST_PILOT_SCORE);
  second.accountId = std::string{SECOND_PILOT_ACCOUNT};
  static_cast<void>(
      sendResult(paris(), FIRST_GAME_ID, normalGameOfTwo({first})));
  static_cast<void>(
      sendResult(paris(), SECOND_GAME_ID, normalGameOfTwo({second})));

  EXPECT_THAT(membersIn<std::string>(leaderboardOf(NORMAL_FOR_TWO_QUERY),
                                     NICKNAME_MEMBER_NAME),
              ElementsAre(OTHER_NICKNAME_OF_SECOND_PILOT));
}

/**
 * Given FIRST_PILOT and SECOND_PILOT with equal scores, and THIRD_PILOT below
 * them When the leaderboard is read Then the two share the first rank, and
 * THIRD_PILOT ranks third
 */
TEST_F(LeaderboardRoutesFixture, EqualScoresShareARank) {
  static_cast<void>(
      sendResult(paris(), FIRST_GAME_ID,
                 normalGameOfTwo({guestScoring(FIRST_PILOT, MIDDLE_SCORE),
                                  guestScoring(SECOND_PILOT, MIDDLE_SCORE)})));
  static_cast<void>(sendResult(
      paris(), SECOND_GAME_ID,
      normalGameOfTwo({guestScoring(THIRD_PILOT, SECOND_PILOT_SCORE)})));

  EXPECT_THAT(membersIn<std::int64_t>(leaderboardOf(NORMAL_FOR_TWO_QUERY),
                                      RANK_MEMBER_NAME),
              ElementsAre(FIRST_RANK, FIRST_RANK, THIRD_RANK));
}

/**
 * Given FIRST_PILOT, then SECOND_PILOT, then THIRD_PILOT, by score
 * When the first row is read, with THIRD_PILOT asked for in lowercase
 * Then the page holds FIRST_PILOT alone, and the player is THIRD_PILOT, third
 */
TEST_F(LeaderboardRoutesFixture, ThePlayerAskedForComesBeyondThePage) {
  static_cast<void>(
      sendResult(paris(), FIRST_GAME_ID,
                 normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE),
                                  guestScoring(SECOND_PILOT, MIDDLE_SCORE)})));
  static_cast<void>(sendResult(
      paris(), SECOND_GAME_ID,
      normalGameOfTwo({guestScoring(THIRD_PILOT, SECOND_PILOT_SCORE)})));

  const nlohmann::json leaderboard = leaderboardOf(FIRST_ROW_AND_R9_QUERY);

  ASSERT_THAT(membersIn<std::string>(leaderboard, NICKNAME_MEMBER_NAME),
              ElementsAre(FIRST_PILOT));
  ASSERT_TRUE(leaderboard.contains(std::string{PLAYER_MEMBER_NAME}));
  const nlohmann::json& player =
      leaderboard.at(std::string{PLAYER_MEMBER_NAME});
  EXPECT_EQ(player.at(std::string{NICKNAME_MEMBER_NAME}).get<std::string>(),
            THIRD_PILOT);
  EXPECT_EQ(player.at(std::string{RANK_MEMBER_NAME}).get<std::int64_t>(),
            THIRD_RANK);
}

/**
 * Given a master
 * When the leaderboard is read without a difficulty, then without a number
 * of players
 * Then each is answered 400
 */
TEST_F(LeaderboardRoutesFixture, ALeaderboardNeedsADifficultyAndAPlayerCount) {
  for (const std::string_view query :
       {WITHOUT_DIFFICULTY_QUERY, WITHOUT_PLAYER_COUNT_QUERY}) {
    SCOPED_TRACE(query);
    const httplib::Result answer = readLeaderboard(query);

    ASSERT_TRUE(answer);
    EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
  }
}

/**
 * Given a master
 * When the leaderboard is read for five players, for server 0, then for a
 * nickname of 17 bytes
 * Then each is answered 400
 */
TEST_F(LeaderboardRoutesFixture, AQueryOutOfItsBoundsIsRefused) {
  for (const std::string_view query :
       {FIVE_PLAYERS_QUERY, SERVER_OF_ZERO_QUERY, PLAYER_TOO_LONG_QUERY}) {
    SCOPED_TRACE(query);
    const httplib::Result answer = readLeaderboard(query);

    ASSERT_TRUE(answer);
    EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
  }
}
