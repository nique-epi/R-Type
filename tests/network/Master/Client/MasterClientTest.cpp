#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "CreatedServer.hpp"
#include "FinishedGame.hpp"
#include "GameListing.hpp"
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardQuery.hpp"
#include "HttpPortProbe.hpp"
#include "MasterAccess.hpp"
#include "MasterClient.hpp"
#include "MasterClientFixture.hpp"
#include "MasterClientTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "NetworkException.hpp"
#include "RunningMaster.hpp"
#include "ServerCreation.hpp"
#include "ServerListing.hpp"

using rtype::network::CreatedServer;
using rtype::network::FinishedGame;
using rtype::network::GameListing;
using rtype::network::GlobalLeaderboard;
using rtype::network::GlobalLeaderboardQuery;
using rtype::network::InvalidMasterUrlException;
using rtype::network::isWellFormedServerKey;
using rtype::network::MasterAccess;
using rtype::network::MasterClient;
using rtype::network::MasterRefusedException;
using rtype::network::MasterUnreachableException;
using rtype::network::ServerCreation;
using rtype::network::ServerListing;
using rtype::network::testing::PARIS_GAME_ID;
using rtype::network::testing::parisFinishedGame;
using rtype::network::testing::parisGameListing;
using rtype::network::testing::parisHeartbeat;
using rtype::network::testing::parisRegistration;

/**
 * Given a running master
 * When a client with no key creates "Paris 1"
 * Then it gets that server and a well-formed key
 */
TEST_F(MasterClientFixture, ACreationGivesTheServerAndItsKey) {
  const CreatedServer created = createdParis();

  EXPECT_EQ(created.name, TEST_SERVER_NAME);
  EXPECT_TRUE(isWellFormedServerKey(created.key));
}

/**
 * Given a created server
 * When a client with its key registers it, then sends its heartbeat
 * Then the master accepts both
 */
TEST_F(MasterClientFixture, ARegisteredServerSendsItsHeartbeat) {
  MasterClient client = clientWith(createdParis().key);

  client.registerServer(parisRegistration());

  EXPECT_NO_THROW(client.sendHeartbeat(parisHeartbeat()));
}

/**
 * Given a running master
 * When a client registers with a key the master never issued
 * Then the master refuses it with 401
 */
TEST_F(MasterClientFixture, AKeyNeverIssuedIsRefusedWith401) {
  MasterClient client = clientWith(std::string{UNKNOWN_SERVER_KEY});

  const MasterRefusedException refusal =
      refusalOf([&client] { client.registerServer(parisRegistration()); });

  EXPECT_EQ(refusal.status(), EXPECTED_UNAUTHORIZED_STATUS);
}

/**
 * Given a created server that has not registered
 * When a client with its key sends its heartbeat
 * Then the master refuses it with 409, and the refusal carries its detail
 */
TEST_F(MasterClientFixture, AHeartbeatBeforeTheRegistrationIsRefusedWith409) {
  MasterClient client = clientWith(createdParis().key);

  const MasterRefusedException refusal =
      refusalOf([&client] { client.sendHeartbeat(parisHeartbeat()); });

  EXPECT_EQ(refusal.status(), EXPECTED_CONFLICT_STATUS);
  EXPECT_NE(std::string{refusal.what()}.find(UNREGISTERED_DETAIL_TEXT),
            std::string::npos);
}

/**
 * Given a created server
 * When a client with its key sends the result of a vanilla game of two
 * players
 * Then the master keeps a row for each player of that game
 */
TEST_F(MasterClientFixture, AServerSendsTheResultOfAGame) {
  MasterClient client = clientWith(createdParis().key);

  client.sendResult(std::string{PARIS_GAME_ID}, parisFinishedGame());

  EXPECT_EQ(master().keptPlayersOf(PARIS_GAME_ID), PLAYERS_OF_THE_PARIS_GAME);
}

/**
 * Given a created server whose result the master already keeps
 * When the client sends that result again
 * Then the master accepts it again, and still keeps one row per player
 */
TEST_F(MasterClientFixture, AResultSentAgainIsAcceptedAndKeptOnce) {
  MasterClient client = clientWith(createdParis().key);
  client.sendResult(std::string{PARIS_GAME_ID}, parisFinishedGame());

  EXPECT_NO_THROW(
      client.sendResult(std::string{PARIS_GAME_ID}, parisFinishedGame()));

  EXPECT_EQ(master().keptPlayersOf(PARIS_GAME_ID), PLAYERS_OF_THE_PARIS_GAME);
}

/**
 * Given a server that sent the result of a vanilla game of ACE and BYDO_HUNTER
 * When a client with no key reads the leaderboard of that game, naming
 * BYDO_HUNTER
 * Then it gets both rows, best first, and the row of BYDO_HUNTER
 */
TEST_F(MasterClientFixture, AClientReadsTheLeaderboardOfTheMaster) {
  const CreatedServer paris = createdParis();
  clientWith(paris.key).sendResult(std::string{PARIS_GAME_ID},
                                   parisFinishedGame());

  const GlobalLeaderboard read =
      clientWith(std::string{}).readLeaderboard(parisGameLeaderboardQuery());

  EXPECT_EQ(read, parisGameLeaderboardOf(paris));
}

/**
 * Given a server that sent the result of a vanilla game of ACE and BYDO_HUNTER
 * When a client reads the leaderboard naming no difficulty and no number of
 * players
 * Then it gets the same rows: the client leaves both out of its query, and the
 * master reads every game
 */
TEST_F(MasterClientFixture, AQueryWithoutFiltersReadsEveryGame) {
  const CreatedServer paris = createdParis();
  clientWith(paris.key).sendResult(std::string{PARIS_GAME_ID},
                                   parisFinishedGame());
  GlobalLeaderboardQuery everyGame = parisGameLeaderboardQuery();
  everyGame.difficulty = std::nullopt;
  everyGame.playerCount = std::nullopt;

  const GlobalLeaderboard read =
      clientWith(std::string{}).readLeaderboard(everyGame);

  EXPECT_EQ(read, parisGameLeaderboardOf(paris));
}

/**
 * Given a created server
 * When a client with its key sends the result of a game that is not vanilla
 * Then the master refuses it with 400 and keeps nothing of it
 */
TEST_F(MasterClientFixture, AGameThatIsNotVanillaIsRefusedWith400) {
  MasterClient client = clientWith(createdParis().key);
  FinishedGame modded = parisFinishedGame();
  modded.vanilla = false;

  const MasterRefusedException refusal = refusalOf([&client, &modded] {
    client.sendResult(std::string{PARIS_GAME_ID}, modded);
  });

  EXPECT_EQ(refusal.status(), EXPECTED_BAD_REQUEST_STATUS);
  EXPECT_EQ(master().keptPlayersOf(PARIS_GAME_ID), 0);
}

/**
 * Given a port nothing listens on
 * When a client creates a server there
 * Then it throws MasterUnreachableException
 */
TEST_F(MasterClientFixture, AMasterThatDoesNotListenIsUnreachable) {
  MasterClient client{MasterAccess{.url = RunningMaster::urlOf(freeHttpPort()),
                                   .key = std::string{}}};

  EXPECT_THROW(static_cast<void>(client.createServer(
                   ServerCreation{.name = std::string{TEST_SERVER_NAME}})),
               MasterUnreachableException);
}

/**
 * Given the https url of a master behind TLS
 * When a client is built for it
 * Then it is built, and speaks TLS to that master
 */
TEST(MasterClient, AnHttpsUrlBuildsAClient) {
  EXPECT_NO_THROW(MasterClient(MasterAccess{
      .url = std::string{HTTPS_MASTER_URL}, .key = std::string{}}));
}

/**
 * Given a url of the master whose port is past 65535
 * When a client is built for it
 * Then it throws InvalidMasterUrlException, instead of holding no connection
 */
TEST(MasterClient, AUrlWithAPortPastTheLastIsRefused) {
  EXPECT_THROW(
      MasterClient(MasterAccess{.url = std::string{PORT_TOO_HIGH_MASTER_URL},
                                .key = std::string{}}),
      InvalidMasterUrlException);
}

/**
 * Given a registered server that sent its heartbeat
 * When a client with no key lists its games
 * Then it reads the games of that heartbeat
 */
TEST_F(MasterClientFixture, AClientWithNoKeyListsTheGamesOfAServer) {
  const CreatedServer created = createdParis();
  MasterClient server = clientWith(created.key);
  server.registerServer(parisRegistration());
  server.sendHeartbeat(parisHeartbeat());
  MasterClient player = clientWith(std::string{});

  const GameListing listing = player.listGames(created.id);

  EXPECT_EQ(listing, parisGameListing());
}

/**
 * Given a running master where no server was created
 * When a client with no key lists the servers
 * Then the list is empty
 */
TEST_F(MasterClientFixture, AMasterWithNoServerListsNone) {
  MasterClient player = clientWith(std::string{});

  EXPECT_TRUE(player.listServers().servers.empty());
}

/**
 * Given "Paris 1", created then registered
 * When a client with no key lists the servers
 * Then it reads that server alone, by the id its creation gave
 */
TEST_F(MasterClientFixture, AClientWithNoKeyListsTheRegisteredServer) {
  const CreatedServer created = createdParis();
  MasterClient server = clientWith(created.key);
  server.registerServer(parisRegistration());
  MasterClient player = clientWith(std::string{});

  const ServerListing listing = player.listServers();

  ASSERT_EQ(listing.servers.size(), 1U);
  EXPECT_EQ(listing.servers.front().id, created.id);
}

/**
 * Given a running master with no server of this id
 * When a client lists the games of that id
 * Then the master refuses it with 404
 */
TEST_F(MasterClientFixture, TheGamesOfAnUnknownServerAreRefusedWith404) {
  MasterClient player = clientWith(std::string{});

  const MasterRefusedException refusal =
      refusalOf([&player] { static_cast<void>(player.listGames(UNKNOWN_ID)); });

  EXPECT_EQ(refusal.status(), EXPECTED_NOT_FOUND);
}
