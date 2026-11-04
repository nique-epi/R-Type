#include <gtest/gtest.h>
#include <string>
#include "CreatedServer.hpp"
#include "HttpPortProbe.hpp"
#include "MasterAccess.hpp"
#include "MasterClient.hpp"
#include "MasterClientFixture.hpp"
#include "MasterClientTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterTestConstants.hpp"
#include "NetworkException.hpp"
#include "RunningMaster.hpp"
#include "ServerCreation.hpp"

using rtype::network::CreatedServer;
using rtype::network::GameListing;
using rtype::network::InvalidMasterUrlException;
using rtype::network::isWellFormedServerKey;
using rtype::network::MasterAccess;
using rtype::network::MasterClient;
using rtype::network::MasterRefusedException;
using rtype::network::MasterUnreachableException;
using rtype::network::ServerCreation;
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
 * Given a url of the master over HTTPS
 * When a client is built for it
 * Then it throws InvalidMasterUrlException, since it speaks no TLS
 */
TEST(MasterClient, AnHttpsUrlIsRefused) {
  EXPECT_THROW(MasterClient(MasterAccess{.url = std::string{HTTPS_MASTER_URL},
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
