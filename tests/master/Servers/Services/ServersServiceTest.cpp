#include <gtest/gtest.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include "CreatedServer.hpp"
#include "KnownServer.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterTestConstants.hpp"
#include "NetworkException.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerKey.hpp"
#include "ServerLimits.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersServiceFixture.hpp"
#include "ServersServiceTestConstants.hpp"

using rtype::master::fingerprintOf;
using rtype::master::KnownServer;
using rtype::master::ServerLimits;
using rtype::master::ServerRecord;
using rtype::network::CreatedServer;
using rtype::network::heartbeatText;
using rtype::network::InvalidMasterMessageException;
using rtype::network::isWellFormedServerKey;
using rtype::network::ServerHeartbeat;
using rtype::network::ServerRegistration;
using rtype::network::testing::parisHeartbeat;
using rtype::network::testing::parisRegistration;

/**
 * Given a master with no server
 * When a host creates one
 * Then it gets the name it asked for and a well-formed key
 */
TEST_F(ServersServiceFixture, ACreatedServerGetsAWellFormedKey) {
  const CreatedServer created = createdServer();

  EXPECT_EQ(created.name, TEST_SERVER_NAME);
  EXPECT_TRUE(isWellFormedServerKey(created.key));
}

/**
 * Given a created server
 * When its key is looked up
 * Then it finds that server, with no limits since it has not registered
 */
TEST_F(ServersServiceFixture, TheKeyOfACreatedServerFindsIt) {
  const CreatedServer created = createdServer();

  const std::optional<KnownServer> found = service().serverOfKey(created.key);

  ASSERT_TRUE(found.has_value());
  const KnownServer server = found.value_or(KnownServer{});
  EXPECT_EQ(server.id, created.id);
  EXPECT_FALSE(server.limits.has_value());
}

/**
 * Given a created server
 * When the database is read
 * Then it holds the fingerprint of the key, never the key
 */
TEST_F(ServersServiceFixture, TheDatabaseKeepsOnlyTheFingerprintOfTheKey) {
  const CreatedServer created = createdServer();

  EXPECT_EQ(storedFingerprintOf(created.id), fingerprintOf(created.key));
  EXPECT_NE(storedFingerprintOf(created.id), created.key);
}

/**
 * Given a created server
 * When a well-formed key the master never issued is looked up
 * Then it finds no server
 */
TEST_F(ServersServiceFixture, AKeyNeverIssuedFindsNoServer) {
  static_cast<void>(createdServer());

  EXPECT_FALSE(service().serverOfKey(UNKNOWN_SERVER_KEY).has_value());
}

/**
 * Given a created server
 * When a text that is no key is looked up
 * Then it finds no server
 */
TEST_F(ServersServiceFixture, AMalformedKeyFindsNoServer) {
  static_cast<void>(createdServer());

  EXPECT_FALSE(service().serverOfKey(MALFORMED_KEY).has_value());
}

/**
 * Given a created server
 * When a second one is created
 * Then they are two servers with two keys
 */
TEST_F(ServersServiceFixture, TwoCreationsAreTwoServersWithTwoKeys) {
  const CreatedServer first = createdServer();

  const CreatedServer second = createdServer();

  EXPECT_NE(second.id, first.id);
  EXPECT_NE(second.key, first.key);
}

/**
 * Given an address that created five servers in this hour
 * When it creates a sixth one
 * Then the creation is refused
 */
TEST_F(ServersServiceFixture, AnAddressCreatesFiveServersAnHour) {
  for (std::size_t creation = 0; creation < CREATIONS_PER_HOUR; ++creation) {
    ASSERT_TRUE(creationAt(CREATOR_ADDRESS, FIRST_HEARTBEAT_DATE).has_value());
  }

  EXPECT_FALSE(creationAt(CREATOR_ADDRESS, FIRST_HEARTBEAT_DATE).has_value());
}

/**
 * Given an address that created five servers at once
 * When it creates another one exactly an hour later
 * Then the creation is still refused, the five being within the hour
 */
TEST_F(ServersServiceFixture, TheQuotaStillCountsACreationOfAnHourAgo) {
  for (std::size_t creation = 0; creation < CREATIONS_PER_HOUR; ++creation) {
    ASSERT_TRUE(creationAt(CREATOR_ADDRESS, FIRST_HEARTBEAT_DATE).has_value());
  }

  EXPECT_FALSE(
      creationAt(CREATOR_ADDRESS, FIRST_HEARTBEAT_DATE + ONE_HOUR).has_value());
}

/**
 * Given an address that created five servers at once
 * When it creates another one an hour and a second later
 * Then the creation is accepted
 */
TEST_F(ServersServiceFixture, TheQuotaComesBackAfterAnHour) {
  for (std::size_t creation = 0; creation < CREATIONS_PER_HOUR; ++creation) {
    ASSERT_TRUE(creationAt(CREATOR_ADDRESS, FIRST_HEARTBEAT_DATE).has_value());
  }

  EXPECT_TRUE(
      creationAt(CREATOR_ADDRESS, FIRST_HEARTBEAT_DATE + ONE_HOUR + ONE_SECOND)
          .has_value());
}

/**
 * Given an address that created five servers in this hour
 * When another address creates one
 * Then the creation is accepted
 */
TEST_F(ServersServiceFixture, TheQuotaIsKeptPerAddress) {
  for (std::size_t creation = 0; creation < CREATIONS_PER_HOUR; ++creation) {
    ASSERT_TRUE(creationAt(CREATOR_ADDRESS, FIRST_HEARTBEAT_DATE).has_value());
  }

  EXPECT_TRUE(
      creationAt(OTHER_CREATOR_ADDRESS, FIRST_HEARTBEAT_DATE).has_value());
}

/**
 * Given a created server
 * When it registers
 * Then its key finds the bounds it registered
 */
TEST_F(ServersServiceFixture, ARegistrationSetsTheLimitsOfTheServer) {
  const KnownServer server = registeredServer(parisRegistration());

  ASSERT_TRUE(server.limits.has_value());
  const ServerLimits limits = server.limits.value_or(ServerLimits{});
  EXPECT_EQ(limits.maximumGames, parisRegistration().maximumGames);
  EXPECT_EQ(limits.maximumPlayersPerGame,
            parisRegistration().maximumPlayersPerGame);
}

/**
 * Given a registered server
 * When it registers again with another name
 * Then it takes the new name
 */
TEST_F(ServersServiceFixture, ARegistrationAgainRenamesTheServer) {
  const KnownServer server = registeredServer(parisRegistration());
  ServerRegistration renamed = parisRegistration();
  renamed.name = std::string{RENAMED_SERVER};

  service().replaceOwnServer(
      ServerRecord{.serverId = server.id,
                   .registration = renamed,
                   .registeredAt = FIRST_HEARTBEAT_DATE});

  EXPECT_EQ(nameOfServer(server.id), RENAMED_SERVER);
}

/**
 * Given a created server that has not registered
 * When its heartbeat comes
 * Then it is refused and no sample is kept
 */
TEST_F(ServersServiceFixture, AHeartbeatBeforeTheRegistrationIsRefused) {
  const CreatedServer created = createdServer();
  const KnownServer server =
      service().serverOfKey(created.key).value_or(KnownServer{});

  EXPECT_FALSE(service().replaceOwnHeartbeat(server, parisHeartbeat(),
                                             FIRST_HEARTBEAT_DATE));
  EXPECT_EQ(sampleCount(), 0);
}

/**
 * Given a registered server
 * When its heartbeat comes
 * Then the heartbeat is kept as the last state of the server
 */
TEST_F(ServersServiceFixture, AHeartbeatIsTheLastStateOfItsServer) {
  const KnownServer server = registeredServer(parisRegistration());

  ASSERT_TRUE(service().replaceOwnHeartbeat(server, parisHeartbeat(),
                                            FIRST_HEARTBEAT_DATE));

  EXPECT_EQ(lastHeartbeatOf(server.id), heartbeatText(parisHeartbeat()));
}

/**
 * Given a registered server
 * When two heartbeats come in one second, then one a second later
 * Then one sample is kept for the first second, and one for the next
 */
TEST_F(ServersServiceFixture, OneSampleIsKeptPerSecond) {
  const KnownServer server = registeredServer(parisRegistration());

  ASSERT_TRUE(service().replaceOwnHeartbeat(server, parisHeartbeat(),
                                            FIRST_HEARTBEAT_DATE));
  ASSERT_TRUE(service().replaceOwnHeartbeat(server, parisHeartbeat(),
                                            FIRST_HEARTBEAT_DATE));
  EXPECT_EQ(sampleCount(), ONE_SAMPLE);

  ASSERT_TRUE(service().replaceOwnHeartbeat(server, parisHeartbeat(),
                                            FIRST_HEARTBEAT_DATE + ONE_SECOND));
  EXPECT_EQ(sampleCount(), TWO_SAMPLES);
}

/**
 * Given a sample taken seven days ago, then one taken eight days ago
 * When a heartbeat comes now
 * Then the sample of seven days is kept and the one of eight days is erased
 */
TEST_F(ServersServiceFixture, ASampleIsKeptSevenDays) {
  const KnownServer server = registeredServer(parisRegistration());
  ASSERT_TRUE(service().replaceOwnHeartbeat(server, parisHeartbeat(),
                                            FIRST_HEARTBEAT_DATE));

  ASSERT_TRUE(service().replaceOwnHeartbeat(server, parisHeartbeat(),
                                            FIRST_HEARTBEAT_DATE + SEVEN_DAYS));
  EXPECT_EQ(sampleCount(), TWO_SAMPLES);

  ASSERT_TRUE(service().replaceOwnHeartbeat(server, parisHeartbeat(),
                                            FIRST_HEARTBEAT_DATE + EIGHT_DAYS));
  EXPECT_EQ(sampleCount(), TWO_SAMPLES);
}

/**
 * Given a server that sent a heartbeat
 * When it registers again, as it does when it restarts
 * Then its last state is forgotten, since it hosts no game yet
 */
TEST_F(ServersServiceFixture, ARegistrationForgetsTheLastHeartbeat) {
  const KnownServer server = registeredServer(parisRegistration());
  ASSERT_TRUE(service().replaceOwnHeartbeat(server, parisHeartbeat(),
                                            FIRST_HEARTBEAT_DATE));

  service().replaceOwnServer(
      ServerRecord{.serverId = server.id,
                   .registration = parisRegistration(),
                   .registeredAt = FIRST_HEARTBEAT_DATE});

  EXPECT_EQ(lastHeartbeatOf(server.id), std::nullopt);
}

/**
 * Given a server registered for one game
 * When its heartbeat lists two games, each numbered 1 and of one player
 * Then it is refused and nothing is kept
 */
TEST_F(ServersServiceFixture, MoreGamesThanRegisteredAreRefused) {
  const KnownServer server = registeredServer(smallRegistration());
  ServerHeartbeat twoGames = parisHeartbeat();
  twoGames.games.back() = twoGames.games.front();

  EXPECT_THROW(static_cast<void>(service().replaceOwnHeartbeat(
                   server, twoGames, FIRST_HEARTBEAT_DATE)),
               InvalidMasterMessageException);
  EXPECT_EQ(sampleCount(), 0);
}

/**
 * Given a server registered for one game
 * When its heartbeat lists one game numbered 2, of one player
 * Then it is refused and nothing is kept
 */
TEST_F(ServersServiceFixture, AGameNumberedAboveTheRegisteredGamesIsRefused) {
  const KnownServer server = registeredServer(smallRegistration());
  ServerHeartbeat secondGame = parisHeartbeat();
  secondGame.games.back().players.pop_back();
  secondGame.games.erase(secondGame.games.begin());

  EXPECT_THROW(static_cast<void>(service().replaceOwnHeartbeat(
                   server, secondGame, FIRST_HEARTBEAT_DATE)),
               InvalidMasterMessageException);
  EXPECT_EQ(sampleCount(), 0);
}

/**
 * Given a server registered for games of one player
 * When its heartbeat lists one game numbered 1, of two players
 * Then it is refused and nothing is kept
 */
TEST_F(ServersServiceFixture, MorePlayersThanRegisteredAreRefused) {
  const KnownServer server = registeredServer(smallRegistration());
  ServerHeartbeat twoPlayers = parisHeartbeat();
  twoPlayers.games.erase(twoPlayers.games.begin());
  twoPlayers.games.front().number = ONE_GAME;

  EXPECT_THROW(static_cast<void>(service().replaceOwnHeartbeat(
                   server, twoPlayers, FIRST_HEARTBEAT_DATE)),
               InvalidMasterMessageException);
  EXPECT_EQ(sampleCount(), 0);
}

/**
 * Given eight registered servers
 * When each sends fifty heartbeats, all at once, each in its own second
 * Then every heartbeat is kept, and none fails on the lock of the database
 */
TEST_F(ServersServiceFixture, EightServersBeatAtOnce) {
  std::vector<KnownServer> registered;
  registered.reserve(PARALLEL_SERVERS);
  for (std::size_t server = 0; server < PARALLEL_SERVERS; ++server) {
    registered.push_back(registeredServer(parisRegistration()));
  }
  std::atomic<std::size_t> failures{0};

  {
    std::vector<std::jthread> servers;
    servers.reserve(registered.size());
    for (const KnownServer& server : registered) {
      servers.emplace_back([this, server, &failures] {
        for (std::size_t beat = 0; beat < HEARTBEATS_PER_SERVER; ++beat) {
          try {
            static_cast<void>(service().replaceOwnHeartbeat(
                server, parisHeartbeat(),
                FIRST_HEARTBEAT_DATE + (ONE_SECOND * beat)));
          } catch (const std::exception&) {
            ++failures;
          }
        }
      });
    }
  }

  EXPECT_EQ(failures, 0U);
  EXPECT_EQ(sampleCount(), static_cast<std::int64_t>(PARALLEL_SERVERS *
                                                     HEARTBEATS_PER_SERVER));
}

/**
 * Given a created server
 * When the master restarts
 * Then its key still finds it
 */
TEST_F(ServersServiceFixture, AServerIsKnownAcrossARestart) {
  const CreatedServer created = createdServer();

  restart();

  EXPECT_EQ(service().serverOfKey(created.key).value_or(KnownServer{}).id,
            created.id);
}
