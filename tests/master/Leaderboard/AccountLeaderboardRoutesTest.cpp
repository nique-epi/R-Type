#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include "AccountLeaderboardRoutesTestConstants.hpp"
#include "AccountsTestConstants.hpp"
#include "FinishedGame.hpp"
#include "GameCreditRoutesFixture.hpp"
#include "GameCreditRoutesTestConstants.hpp"
#include "LeaderboardRoutesFixtures.hpp"
#include "LeaderboardRoutesTestConstants.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "MasterTestConstants.hpp"

using rtype::network::FinishedGame;
using rtype::network::testing::FIRST_PILOT;
using rtype::network::testing::FIRST_PILOT_SCORE;

/**
 * Given ACE, who scored 1234 in a normal game for two with a ticket
 * When ACE scores 1000 in a hard game alone, with another ticket
 * Then the leaderboard of the accounts gives ACE the total of both, 2234
 */
TEST_F(GameCreditRoutesFixture, TheAccountLeaderboardAddsUpEveryGame) {
  static_cast<void>(sendResult(
      FIRST_GAME_ID,
      normalGameOfTwo({ticketedScoring(ACE_NICKNAME, aceTicket(),
                                       TWELVE_HUNDRED_THIRTY_FOUR_POINTS)})));
  FinishedGame hardAlone = normalGameOfTwo(
      {ticketedScoring(ACE_NICKNAME, aceTicket(), A_THOUSAND_POINTS)});
  hardAlone.difficulty = std::string{HARD_DIFFICULTY};
  hardAlone.playerCount = ONE_PLAYER;

  static_cast<void>(sendResult(SECOND_GAME_ID, hardAlone));

  EXPECT_EQ(membersOf<std::int64_t>(
                bodyOf(readAccountLeaderboard(FIRST_PAGE_OF_ACCOUNTS_QUERY)),
                ACCOUNTS_MEMBER_NAME, SCORE_MEMBER_NAME),
            std::vector<std::int64_t>{TOTAL_OF_1234_AND_A_THOUSAND});
}

/**
 * Given BOB, whose one game scored 2000, more than any game of ACE
 * When ACE scores 1234 then 1000, 2234 in all
 * Then ACE ranks before BOB: the total counts, not the best game
 */
TEST_F(GameCreditRoutesFixture, TheAccountLeaderboardRanksByTheTotal) {
  static_cast<void>(sendResult(
      FIRST_GAME_ID, normalGameOfTwo({ticketedScoring(BOB_NICKNAME, bobTicket(),
                                                      TWO_THOUSAND_POINTS)})));

  static_cast<void>(sendResult(
      SECOND_GAME_ID,
      normalGameOfTwo({ticketedScoring(ACE_NICKNAME, aceTicket(),
                                       TWELVE_HUNDRED_THIRTY_FOUR_POINTS)})));
  static_cast<void>(sendResult(
      THIRD_GAME_ID, normalGameOfTwo({ticketedScoring(ACE_NICKNAME, aceTicket(),
                                                      A_THOUSAND_POINTS)})));

  EXPECT_EQ(membersOf<std::string>(
                bodyOf(readAccountLeaderboard(FIRST_PAGE_OF_ACCOUNTS_QUERY)),
                ACCOUNTS_MEMBER_NAME, NICKNAME_MEMBER_NAME),
            (std::vector<std::string>{std::string{ACE_NICKNAME},
                                      std::string{BOB_NICKNAME}}));
}

/**
 * Given the accounts ACE and BOB, who played no game
 * When a guest scores in a game
 * Then the leaderboard of the accounts lists nobody
 */
TEST_F(GameCreditRoutesFixture, TheAccountLeaderboardLeavesGuestsOut) {
  static_cast<void>(sendResult(
      FIRST_GAME_ID,
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE)})));

  EXPECT_TRUE(bodyOf(readAccountLeaderboard(FIRST_PAGE_OF_ACCOUNTS_QUERY))
                  .at(std::string{ACCOUNTS_MEMBER_NAME})
                  .empty());
}

/**
 * Given a ticket of ACE
 * When ACE plays with it under the nickname HUNTER_2
 * Then the leaderboard of the accounts names it ACE, the nickname of the
 * account
 */
TEST_F(GameCreditRoutesFixture, TheAccountLeaderboardNamesTheAccount) {
  static_cast<void>(sendResult(
      FIRST_GAME_ID,
      normalGameOfTwo({ticketedScoring(OTHER_NICKNAME_OF_SECOND_PILOT,
                                       aceTicket(), A_THOUSAND_POINTS)})));

  EXPECT_EQ(membersOf<std::string>(
                bodyOf(readAccountLeaderboard(FIRST_PAGE_OF_ACCOUNTS_QUERY)),
                ACCOUNTS_MEMBER_NAME, NICKNAME_MEMBER_NAME),
            std::vector<std::string>{std::string{ACE_NICKNAME}});
}

/**
 * Given a game of ACE its ticket paid
 * When another game of ACE names the same ticket, which pays nothing more
 * Then only the first game counts for ACE: 1234
 */
TEST_F(GameCreditRoutesFixture,
       TheAccountLeaderboardLeavesOutAGameItsTicketDidNotPay) {
  const std::string ticket = aceTicket();
  static_cast<void>(sendResult(
      FIRST_GAME_ID,
      normalGameOfTwo({ticketedScoring(ACE_NICKNAME, ticket,
                                       TWELVE_HUNDRED_THIRTY_FOUR_POINTS)})));

  static_cast<void>(sendResult(
      SECOND_GAME_ID, normalGameOfTwo({ticketedScoring(ACE_NICKNAME, ticket,
                                                       A_THOUSAND_POINTS)})));

  EXPECT_EQ(membersOf<std::int64_t>(
                bodyOf(readAccountLeaderboard(FIRST_PAGE_OF_ACCOUNTS_QUERY)),
                ACCOUNTS_MEMBER_NAME, SCORE_MEMBER_NAME),
            std::vector<std::int64_t>{
                std::int64_t{TWELVE_HUNDRED_THIRTY_FOUR_POINTS}});
}

/**
 * Given a game of ACE that ended at GAME_DATE
 * When a game of ACE ends a day later
 * Then the row of ACE gives the date of the later game
 */
TEST_F(GameCreditRoutesFixture, TheAccountLeaderboardDatesTheLastGame) {
  static_cast<void>(sendResult(
      FIRST_GAME_ID, normalGameOfTwo({ticketedScoring(ACE_NICKNAME, aceTicket(),
                                                      A_THOUSAND_POINTS)})));
  FinishedGame later = normalGameOfTwo(
      {ticketedScoring(ACE_NICKNAME, aceTicket(), A_THOUSAND_POINTS)});
  later.playedAt = LATER_GAME_DATE;

  static_cast<void>(sendResult(SECOND_GAME_ID, later));

  EXPECT_EQ(membersOf<std::int64_t>(
                bodyOf(readAccountLeaderboard(FIRST_PAGE_OF_ACCOUNTS_QUERY)),
                ACCOUNTS_MEMBER_NAME, LAST_PLAYED_AT_MEMBER_NAME),
            std::vector<std::int64_t>{LATER_GAME_SECOND});
}

/**
 * Given ACE, who scored 1000
 * When BOB scores 1000 too
 * Then both rank first
 */
TEST_F(GameCreditRoutesFixture, TheAccountLeaderboardSharesARankBetweenEquals) {
  static_cast<void>(sendResult(
      FIRST_GAME_ID, normalGameOfTwo({ticketedScoring(ACE_NICKNAME, aceTicket(),
                                                      A_THOUSAND_POINTS)})));

  static_cast<void>(sendResult(
      SECOND_GAME_ID, normalGameOfTwo({ticketedScoring(
                          BOB_NICKNAME, bobTicket(), A_THOUSAND_POINTS)})));

  EXPECT_EQ(membersOf<std::int64_t>(
                bodyOf(readAccountLeaderboard(FIRST_PAGE_OF_ACCOUNTS_QUERY)),
                ACCOUNTS_MEMBER_NAME, RANK_MEMBER_NAME),
            (std::vector<std::int64_t>{BEST_RANK, BEST_RANK}));
}

/**
 * Given ACE, who scored 1234, and BOB, who scored 1000
 * When the leaderboard of the accounts is read one row after its first
 * Then it holds BOB alone
 */
TEST_F(GameCreditRoutesFixture, TheAccountLeaderboardIsReadAPageAtATime) {
  static_cast<void>(sendResult(
      FIRST_GAME_ID,
      normalGameOfTwo({ticketedScoring(ACE_NICKNAME, aceTicket(),
                                       TWELVE_HUNDRED_THIRTY_FOUR_POINTS)})));
  static_cast<void>(sendResult(
      SECOND_GAME_ID, normalGameOfTwo({ticketedScoring(
                          BOB_NICKNAME, bobTicket(), A_THOUSAND_POINTS)})));

  EXPECT_EQ(membersOf<std::string>(
                bodyOf(readAccountLeaderboard(SECOND_ACCOUNT_ALONE_QUERY)),
                ACCOUNTS_MEMBER_NAME, NICKNAME_MEMBER_NAME),
            std::vector<std::string>{std::string{BOB_NICKNAME}});
}

/**
 * Given the master
 * When the leaderboard of the accounts is asked a page of no row
 * Then the master refuses it
 */
TEST_F(GameCreditRoutesFixture, TheAccountLeaderboardRefusesAPageOfNoRow) {
  const httplib::Result answer = readAccountLeaderboard(PAGE_OF_NO_ROW_QUERY);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
}
