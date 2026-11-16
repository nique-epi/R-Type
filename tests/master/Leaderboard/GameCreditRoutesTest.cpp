#include <gtest/gtest.h>
#include <httplib.h>
#include <optional>
#include <string>
#include "AccountsRoutesFixtures.hpp"
#include "AccountsTestConstants.hpp"
#include "CreatedServer.hpp"
#include "FinishedGame.hpp"
#include "FinishedGamePlayer.hpp"
#include "GameCreditRoutesFixture.hpp"
#include "GameCreditRoutesTestConstants.hpp"
#include "LeaderboardRoutesFixtures.hpp"
#include "LeaderboardRoutesTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "RunningMaster.hpp"

using rtype::network::FinishedGame;
using rtype::network::FinishedGamePlayer;

/**
 * Given ACE, whose wallet holds no kopeck
 * When "Paris 1" sends a vanilla game where ACE, under its account, scored
 * 1234, then sends it again
 * Then ACE gets 123 kopecks, once
 */
TEST_F(GameCreditRoutesFixture, AGameIsCreditedOnce) {
  const FinishedGame game = normalGameOfTwo(
      {FinishedGamePlayer{.nickname = std::string{ACE_NICKNAME},
                          .accountId = aceAccountId(),
                          .score = TWELVE_HUNDRED_THIRTY_FOUR_POINTS,
                          .wonLooks = {}}});

  ASSERT_EQ(sendResult(FIRST_GAME_ID, game)->status, EXPECTED_NO_CONTENT);
  ASSERT_EQ(sendResult(FIRST_GAME_ID, game)->status, EXPECTED_NO_CONTENT);

  EXPECT_EQ(kopecksOfAce(), std::optional{KOPECKS_OF_1234_POINTS});
}

/**
 * Given ACE, whose wallet holds no kopeck
 * When a won game where ACE scored 1000 and won the party hat comes
 * Then ACE gets 100 kopecks and a quarter more, 125, and owns the hat
 */
TEST_F(GameCreditRoutesFixture, AVictoryPaysABonusAndItsLooks) {
  FinishedGame game = normalGameOfTwo(
      {FinishedGamePlayer{.nickname = std::string{ACE_NICKNAME},
                          .accountId = aceAccountId(),
                          .score = A_THOUSAND_POINTS,
                          .wonLooks = {std::string{WON_PARTY_HAT}}}});
  game.victory = true;

  ASSERT_EQ(sendResult(FIRST_GAME_ID, game)->status, EXPECTED_NO_CONTENT);

  EXPECT_EQ(kopecksOfAce(), std::optional{KOPECKS_OF_A_THOUSAND_POINTS_WON});
  EXPECT_EQ(looksOfAce(), std::optional{LOOKS_WITH_THE_PARTY_HAT});
}

/**
 * Given ACE, whose wallet holds no kopeck
 * When games come whose player names no number of an account, then a number
 * of no account
 * Then each result is kept, and no wallet gets anything
 */
TEST_F(GameCreditRoutesFixture, AnAccountIdOfNoAccountPaysNothing) {
  const FinishedGame noNumber = normalGameOfTwo(
      {FinishedGamePlayer{.nickname = std::string{ACE_NICKNAME},
                          .accountId = std::string{NO_NUMBER_ACCOUNT_ID},
                          .score = A_THOUSAND_POINTS,
                          .wonLooks = {}}});
  const FinishedGame noAccount = normalGameOfTwo(
      {FinishedGamePlayer{.nickname = std::string{ACE_NICKNAME},
                          .accountId = std::string{NO_SUCH_ACCOUNT_ID},
                          .score = A_THOUSAND_POINTS,
                          .wonLooks = {}}});

  EXPECT_EQ(sendResult(FIRST_GAME_ID, noNumber)->status, EXPECTED_NO_CONTENT);
  EXPECT_EQ(sendResult(SECOND_GAME_ID, noAccount)->status, EXPECTED_NO_CONTENT);
  EXPECT_EQ(kopecksOfAce(), std::optional{NO_KOPECK});
}

/**
 * Given ACE, whose wallet holds no kopeck
 * When a game that was not vanilla comes, ACE playing under its account
 * Then the master refuses it, and ACE gets nothing
 */
TEST_F(GameCreditRoutesFixture, AGameThatWasNotVanillaPaysNothing) {
  FinishedGame game =
      normalGameOfTwo({FinishedGamePlayer{.nickname = std::string{ACE_NICKNAME},
                                          .accountId = aceAccountId(),
                                          .score = A_THOUSAND_POINTS,
                                          .wonLooks = {}}});
  game.vanilla = false;

  EXPECT_EQ(sendResult(FIRST_GAME_ID, game)->status, EXPECTED_BAD_REQUEST);
  EXPECT_EQ(kopecksOfAce(), std::optional{NO_KOPECK});
}

/**
 * Given a master whose wallets are off
 * When a server sends a game where ACE played under an account
 * Then the result is kept all the same
 */
TEST(GameCreditRoutes, AMasterWithoutWalletsKeepsTheResults) {
  const RunningMaster master(walletOffLines());
  const rtype::network::CreatedServer paris =
      master.createServer(TEST_SERVER_NAME);
  const std::string path =
      std::string{OWN_RESULT_URL_PREFIX} + std::string{FIRST_GAME_ID};

  const httplib::Result kept = sendRouteCall(
      master.port(),
      RouteCall{
          .method = PUT_METHOD,
          .path = path,
          .key = paris.key,
          .body = rtype::network::finishedGameText(normalGameOfTwo(
              {FinishedGamePlayer{.nickname = std::string{ACE_NICKNAME},
                                  .accountId = std::string{NO_SUCH_ACCOUNT_ID},
                                  .score = A_THOUSAND_POINTS,
                                  .wonLooks = {}}})),
          .overHttps = false});

  ASSERT_TRUE(kept);
  EXPECT_EQ(kept->status, EXPECTED_NO_CONTENT);
  EXPECT_EQ(master.keptPlayersOf(FIRST_GAME_ID), 1);
}
