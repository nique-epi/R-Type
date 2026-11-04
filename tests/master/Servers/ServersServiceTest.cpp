#include <gtest/gtest.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "NetworkException.hpp"
#include "RegisteredServer.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersServiceFixture.hpp"
#include "ServersServiceTestConstants.hpp"

using rtype::master::RegisteredServer;
using rtype::master::ServerRecord;
using rtype::network::heartbeatText;
using rtype::network::InvalidMasterMessageException;
using rtype::network::ServerHeartbeat;
using rtype::network::ServerRegistration;
using rtype::network::testing::parisHeartbeat;
using rtype::network::testing::parisRegistration;

/**
 * Given a key that registered no server
 * When a server registers with it
 * Then the server is created
 */
TEST_F(ServersServiceFixture, AFirstRegistrationCreatesTheServer) {
  const RegisteredServer saved = service().registerServer(
      ServerRecord{.keyId = newKeyId(),
                   .registration = parisRegistration(),
                   .registeredAt = FIRST_HEARTBEAT_DATE});

  EXPECT_TRUE(saved.created);
}

/**
 * Given a server registered under a name
 * When it registers again with the same key and another name
 * Then it keeps its id and takes the new name
 */
TEST_F(ServersServiceFixture, ARegistrationAgainKeepsTheIdAndUpdates) {
  const std::int64_t keyId = newKeyId();
  const RegisteredServer first = service().registerServer(
      ServerRecord{.keyId = keyId,
                   .registration = parisRegistration(),
                   .registeredAt = FIRST_HEARTBEAT_DATE});
  ServerRegistration renamed = parisRegistration();
  renamed.name = std::string{RENAMED_SERVER};

  const RegisteredServer second = service().registerServer(
      ServerRecord{.keyId = keyId,
                   .registration = renamed,
                   .registeredAt = FIRST_HEARTBEAT_DATE});

  EXPECT_FALSE(second.created);
  EXPECT_EQ(second.id, first.id);
  EXPECT_EQ(nameOfServer(first.id), RENAMED_SERVER);
}

/**
 * Given two keys
 * When a server registers with each
 * Then they are two servers
 */
TEST_F(ServersServiceFixture, TwoKeysAreTwoServers) {
  const RegisteredServer first = service().registerServer(
      ServerRecord{.keyId = newKeyId(),
                   .registration = parisRegistration(),
                   .registeredAt = FIRST_HEARTBEAT_DATE});
  const RegisteredServer second = service().registerServer(
      ServerRecord{.keyId = newKeyId(),
                   .registration = parisRegistration(),
                   .registeredAt = FIRST_HEARTBEAT_DATE});

  EXPECT_NE(second.id, first.id);
}

/**
 * Given a key that registered no server
 * When a heartbeat comes with it
 * Then it is refused and no sample is kept
 */
TEST_F(ServersServiceFixture, AHeartbeatOfAKeyWithNoServerIsRefused) {
  EXPECT_FALSE(service().recordHeartbeat(newKeyId(), parisHeartbeat(),
                                         FIRST_HEARTBEAT_DATE));
  EXPECT_EQ(sampleCount(), 0);
}

/**
 * Given a registered server
 * When its heartbeat comes
 * Then the heartbeat is kept as the last state of the server
 */
TEST_F(ServersServiceFixture, AHeartbeatIsTheLastStateOfItsServer) {
  const std::int64_t keyId = newKeyId();
  const RegisteredServer saved = service().registerServer(
      ServerRecord{.keyId = keyId,
                   .registration = parisRegistration(),
                   .registeredAt = FIRST_HEARTBEAT_DATE});

  ASSERT_TRUE(
      service().recordHeartbeat(keyId, parisHeartbeat(), FIRST_HEARTBEAT_DATE));

  EXPECT_EQ(lastHeartbeatOf(saved.id), heartbeatText(parisHeartbeat()));
}

/**
 * Given a registered server
 * When two heartbeats come in one second, then one a second later
 * Then one sample is kept for the first second, and one for the next
 */
TEST_F(ServersServiceFixture, OneSampleIsKeptPerSecond) {
  const std::int64_t keyId = newKeyId();
  static_cast<void>(service().registerServer(
      ServerRecord{.keyId = keyId,
                   .registration = parisRegistration(),
                   .registeredAt = FIRST_HEARTBEAT_DATE}));

  ASSERT_TRUE(
      service().recordHeartbeat(keyId, parisHeartbeat(), FIRST_HEARTBEAT_DATE));
  ASSERT_TRUE(
      service().recordHeartbeat(keyId, parisHeartbeat(), FIRST_HEARTBEAT_DATE));
  EXPECT_EQ(sampleCount(), ONE_SAMPLE);

  ASSERT_TRUE(service().recordHeartbeat(keyId, parisHeartbeat(),
                                        FIRST_HEARTBEAT_DATE + ONE_SECOND));
  EXPECT_EQ(sampleCount(), TWO_SAMPLES);
}

/**
 * Given a sample taken seven days ago, then one taken eight days ago
 * When a heartbeat comes now
 * Then the sample of seven days is kept and the one of eight days is erased
 */
TEST_F(ServersServiceFixture, ASampleIsKeptSevenDays) {
  const std::int64_t keyId = newKeyId();
  static_cast<void>(service().registerServer(
      ServerRecord{.keyId = keyId,
                   .registration = parisRegistration(),
                   .registeredAt = FIRST_HEARTBEAT_DATE}));
  ASSERT_TRUE(
      service().recordHeartbeat(keyId, parisHeartbeat(), FIRST_HEARTBEAT_DATE));

  ASSERT_TRUE(service().recordHeartbeat(keyId, parisHeartbeat(),
                                        FIRST_HEARTBEAT_DATE + SEVEN_DAYS));
  EXPECT_EQ(sampleCount(), TWO_SAMPLES);

  ASSERT_TRUE(service().recordHeartbeat(keyId, parisHeartbeat(),
                                        FIRST_HEARTBEAT_DATE + EIGHT_DAYS));
  EXPECT_EQ(sampleCount(), TWO_SAMPLES);
}

/**
 * Given a server that sent a heartbeat
 * When it registers again, as it does when it restarts
 * Then its last state is forgotten, since it hosts no game yet
 */
TEST_F(ServersServiceFixture, ARegistrationForgetsTheLastHeartbeat) {
  const std::int64_t keyId = newKeyId();
  const ServerRecord record{.keyId = keyId,
                            .registration = parisRegistration(),
                            .registeredAt = FIRST_HEARTBEAT_DATE};
  const RegisteredServer saved = service().registerServer(record);
  ASSERT_TRUE(
      service().recordHeartbeat(keyId, parisHeartbeat(), FIRST_HEARTBEAT_DATE));

  static_cast<void>(service().registerServer(record));

  EXPECT_EQ(lastHeartbeatOf(saved.id), std::nullopt);
}

/**
 * Given a server registered for one game
 * When its heartbeat lists two games, each numbered 1 and of one player
 * Then it is refused and nothing is kept
 */
TEST_F(ServersServiceFixture, MoreGamesThanRegisteredAreRefused) {
  const std::int64_t keyId = keyIdOfASmallServer();
  ServerHeartbeat twoGames = parisHeartbeat();
  twoGames.games.back() = twoGames.games.front();

  EXPECT_THROW(static_cast<void>(service().recordHeartbeat(
                   keyId, twoGames, FIRST_HEARTBEAT_DATE)),
               InvalidMasterMessageException);
  EXPECT_EQ(sampleCount(), 0);
}

/**
 * Given a server registered for one game
 * When its heartbeat lists one game numbered 2, of one player
 * Then it is refused and nothing is kept
 */
TEST_F(ServersServiceFixture, AGameNumberedAboveTheRegisteredGamesIsRefused) {
  const std::int64_t keyId = keyIdOfASmallServer();
  ServerHeartbeat secondGame = parisHeartbeat();
  secondGame.games.back().players.pop_back();
  secondGame.games.erase(secondGame.games.begin());

  EXPECT_THROW(static_cast<void>(service().recordHeartbeat(
                   keyId, secondGame, FIRST_HEARTBEAT_DATE)),
               InvalidMasterMessageException);
  EXPECT_EQ(sampleCount(), 0);
}

/**
 * Given a server registered for games of one player
 * When its heartbeat lists one game numbered 1, of two players
 * Then it is refused and nothing is kept
 */
TEST_F(ServersServiceFixture, MorePlayersThanRegisteredAreRefused) {
  const std::int64_t keyId = keyIdOfASmallServer();
  ServerHeartbeat twoPlayers = parisHeartbeat();
  twoPlayers.games.erase(twoPlayers.games.begin());
  twoPlayers.games.front().number = ONE_GAME;

  EXPECT_THROW(static_cast<void>(service().recordHeartbeat(
                   keyId, twoPlayers, FIRST_HEARTBEAT_DATE)),
               InvalidMasterMessageException);
  EXPECT_EQ(sampleCount(), 0);
}

/**
 * Given eight registered servers
 * When each sends fifty heartbeats, all at once, each in its own second
 * Then every heartbeat is kept, and none fails on the lock of the database
 */
TEST_F(ServersServiceFixture, EightServersBeatAtOnce) {
  std::vector<std::int64_t> keyIds;
  for (std::size_t server = 0; server < PARALLEL_SERVERS; ++server) {
    keyIds.push_back(newKeyId());
    static_cast<void>(service().registerServer(
        ServerRecord{.keyId = keyIds.back(),
                     .registration = parisRegistration(),
                     .registeredAt = FIRST_HEARTBEAT_DATE}));
  }
  std::atomic<std::size_t> failures{0};

  {
    std::vector<std::jthread> servers;
    servers.reserve(keyIds.size());
    for (const std::int64_t keyId : keyIds) {
      servers.emplace_back([this, keyId, &failures] {
        for (std::size_t beat = 0; beat < HEARTBEATS_PER_SERVER; ++beat) {
          try {
            static_cast<void>(service().recordHeartbeat(
                keyId, parisHeartbeat(),
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
 * Given a registered server
 * When the master restarts
 * Then the server is still registered, so its heartbeat is kept
 */
TEST_F(ServersServiceFixture, AServerStaysRegisteredAcrossARestart) {
  const std::int64_t keyId = newKeyId();
  static_cast<void>(service().registerServer(
      ServerRecord{.keyId = keyId,
                   .registration = parisRegistration(),
                   .registeredAt = FIRST_HEARTBEAT_DATE}));

  restart();

  EXPECT_TRUE(
      service().recordHeartbeat(keyId, parisHeartbeat(), FIRST_HEARTBEAT_DATE));
}
