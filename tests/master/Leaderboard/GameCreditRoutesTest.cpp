#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
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

namespace {

/** @returns A vanilla game of two where ACE scored @p score, joined with
 * @p ticket. */
FinishedGame gameOfAceWith(const std::string& ticket, std::uint32_t score) {
  return normalGameOfTwo(
      {FinishedGamePlayer{.nickname = std::string{ACE_NICKNAME},
                          .accountId = std::nullopt,
                          .score = score,
                          .wonLooks = {},
                          .ticket = ticket}});
}

}  // namespace

/**
 * Given ACE, whose wallet holds no kopeck, and a ticket it was given
 * When "Paris 1" sends a vanilla game where ACE, with that ticket, scored
 * 1234, then sends it again
 * Then ACE gets 24 kopecks, once
 */
TEST_F(GameCreditRoutesFixture, AGameIsCreditedOnce) {
  const FinishedGame game =
      gameOfAceWith(aceTicket(), TWELVE_HUNDRED_THIRTY_FOUR_POINTS);

  ASSERT_EQ(sendResult(FIRST_GAME_ID, game)->status, EXPECTED_NO_CONTENT);
  ASSERT_EQ(sendResult(FIRST_GAME_ID, game)->status, EXPECTED_NO_CONTENT);

  EXPECT_EQ(kopecksOfAce(), std::optional{KOPECKS_OF_1234_POINTS});
}

/**
 * Given ACE, whose wallet holds no kopeck
 * When a won game where ACE, with a ticket, scored 1000 and won the party hat
 * comes
 * Then ACE gets the 20 kopecks of its score, no more for the victory, and
 * owns the hat
 */
TEST_F(GameCreditRoutesFixture, AVictoryPaysItsScoreAndItsLooks) {
  FinishedGame game = gameOfAceWith(aceTicket(), A_THOUSAND_POINTS);
  game.players.at(0).wonLooks = {std::string{WON_PARTY_HAT}};
  game.victory = true;

  ASSERT_EQ(sendResult(FIRST_GAME_ID, game)->status, EXPECTED_NO_CONTENT);

  EXPECT_EQ(kopecksOfAce(), std::optional{KOPECKS_OF_A_THOUSAND_POINTS});
  EXPECT_EQ(looksOfAce(), std::optional{LOOKS_WITH_THE_PARTY_HAT});
}

/**
 * Given ACE, whose wallet holds no kopeck
 * When a game comes where ACE, with a ticket, scored 1234 and earned the
 * bounty of the Necromancer, 350
 * Then ACE gets both: 24 kopecks of its score and 350, 374
 */
TEST_F(GameCreditRoutesFixture, TheBountyIsPaidWithTheScore) {
  FinishedGame game =
      gameOfAceWith(aceTicket(), TWELVE_HUNDRED_THIRTY_FOUR_POINTS);
  game.players.at(0).bounty = NECROMANCER_BOUNTY;

  ASSERT_EQ(sendResult(FIRST_GAME_ID, game)->status, EXPECTED_NO_CONTENT);

  EXPECT_EQ(kopecksOfAce(),
            std::optional{KOPECKS_OF_1234_POINTS_AND_THE_NECROMANCER});
}

/**
 * Given ACE, whose wallet holds no kopeck
 * When a game comes whose player names the account id of ACE, and no ticket
 * Then the result is kept, and ACE gets nothing: an id names no one
 */
TEST_F(GameCreditRoutesFixture, AnAccountIdPaysNothing) {
  const FinishedGame game =
      normalGameOfTwo({FinishedGamePlayer{.nickname = std::string{ACE_NICKNAME},
                                          .accountId = aceAccountId(),
                                          .score = A_THOUSAND_POINTS,
                                          .wonLooks = {},
                                          .ticket = std::nullopt}});

  EXPECT_EQ(sendResult(FIRST_GAME_ID, game)->status, EXPECTED_NO_CONTENT);
  EXPECT_EQ(kopecksOfAce(), std::optional{NO_KOPECK});
}

/**
 * Given ACE, whose wallet holds no kopeck
 * When a game comes whose player names a ticket the master never gave
 * Then the result is kept, and no wallet gets anything
 */
TEST_F(GameCreditRoutesFixture, AnUnknownTicketPaysNothing) {
  EXPECT_EQ(
      sendResult(FIRST_GAME_ID, gameOfAceWith(std::string{NEVER_GIVEN_TICKET},
                                              A_THOUSAND_POINTS))
          ->status,
      EXPECTED_NO_CONTENT);
  EXPECT_EQ(kopecksOfAce(), std::optional{NO_KOPECK});
  EXPECT_EQ(kopecksOfBob(), std::optional{NO_KOPECK});
}

/**
 * Given a ticket of ACE that paid a first game of 1234 points
 * When a second game comes with the same ticket
 * Then the second is kept and pays nothing: a ticket pays one result
 */
TEST_F(GameCreditRoutesFixture, AUsedTicketPaysNothing) {
  const std::string ticket = aceTicket();
  ASSERT_EQ(sendResult(FIRST_GAME_ID,
                       gameOfAceWith(ticket, TWELVE_HUNDRED_THIRTY_FOUR_POINTS))
                ->status,
            EXPECTED_NO_CONTENT);

  EXPECT_EQ(sendResult(SECOND_GAME_ID, gameOfAceWith(ticket, A_THOUSAND_POINTS))
                ->status,
            EXPECTED_NO_CONTENT);
  EXPECT_EQ(kopecksOfAce(), std::optional{KOPECKS_OF_1234_POINTS});
}

/**
 * Given ACE and BOB, whose wallets hold no kopeck
 * When a game comes whose player is named ACE, with a ticket of BOB
 * Then BOB gets the kopecks and ACE nothing: the ticket names the account,
 * never the nickname
 */
TEST_F(GameCreditRoutesFixture, ATicketPaysTheAccountItWasGivenTo) {
  ASSERT_EQ(
      sendResult(FIRST_GAME_ID,
                 gameOfAceWith(bobTicket(), TWELVE_HUNDRED_THIRTY_FOUR_POINTS))
          ->status,
      EXPECTED_NO_CONTENT);

  EXPECT_EQ(kopecksOfBob(), std::optional{KOPECKS_OF_1234_POINTS});
  EXPECT_EQ(kopecksOfAce(), std::optional{NO_KOPECK});
}

/**
 * Given ACE, whose wallet holds no kopeck
 * When a game that was not vanilla comes, ACE playing with a ticket
 * Then the master refuses it, and ACE gets nothing
 */
TEST_F(GameCreditRoutesFixture, AGameThatWasNotVanillaPaysNothing) {
  FinishedGame game = gameOfAceWith(aceTicket(), A_THOUSAND_POINTS);
  game.vanilla = false;

  EXPECT_EQ(sendResult(FIRST_GAME_ID, game)->status, EXPECTED_BAD_REQUEST);
  EXPECT_EQ(kopecksOfAce(), std::optional{NO_KOPECK});
}

/**
 * Given a master whose wallets are off
 * When a server sends a game where ACE played with a ticket
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
      RouteCall{.method = PUT_METHOD,
                .path = path,
                .key = paris.key,
                .body = rtype::network::finishedGameText(gameOfAceWith(
                    std::string{NEVER_GIVEN_TICKET}, A_THOUSAND_POINTS)),
                .overHttps = false});

  ASSERT_TRUE(kept);
  EXPECT_EQ(kept->status, EXPECTED_NO_CONTENT);
  EXPECT_EQ(master.keptPlayersOf(FIRST_GAME_ID), 1);
}
