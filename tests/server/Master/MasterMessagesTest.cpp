#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "MasterMessages.hpp"
#include "MasterMessagesTestConstants.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerIdentity.hpp"
#include "ServerMeasures.hpp"
#include "ServerRegistration.hpp"

using rtype::network::ServerHeartbeat;
using rtype::network::ServerMeasures;
using rtype::network::ServerRegistration;
using rtype::server::heartbeatOf;
using rtype::server::registrationOf;
using rtype::server::ServerIdentity;
using rtype::server::testing::ANNOUNCED_PLAYERS_PER_GAME;
using rtype::server::testing::ANNOUNCED_PROTOCOL_VERSION;
using rtype::server::testing::CARRIED_MEDIAN_TICK_MILLISECONDS;
using rtype::server::testing::LINKED_SERVER_GAMES;
using rtype::server::testing::LINKED_SERVER_NAME;
using rtype::server::testing::LINKED_SERVER_PORT;

/**
 * Given a server named, on a port, hosting eight games
 * When its registration is written
 * Then it carries them, protocol 8, four players a game and no address
 */
TEST(MasterMessages, TheRegistrationTellsWhatTheServerIs) {
  const ServerRegistration registration =
      registrationOf(ServerIdentity{.name = std::string{LINKED_SERVER_NAME},
                                    .udpPort = LINKED_SERVER_PORT,
                                    .maximumGames = LINKED_SERVER_GAMES});

  EXPECT_EQ(registration.name, LINKED_SERVER_NAME);
  EXPECT_EQ(registration.udpPort, LINKED_SERVER_PORT);
  EXPECT_EQ(registration.maximumGames, LINKED_SERVER_GAMES);
  EXPECT_EQ(registration.protocolVersion, ANNOUNCED_PROTOCOL_VERSION);
  EXPECT_EQ(registration.maximumPlayersPerGame, ANNOUNCED_PLAYERS_PER_GAME);
  EXPECT_FALSE(registration.address.has_value());
}

/**
 * Given measures with a median tick of 2.5 ms
 * When a heartbeat is written with them
 * Then it carries them as they are
 */
TEST(MasterMessages, AHeartbeatCarriesTheMeasuresItIsGiven) {
  const ServerMeasures measures{
      .tickP50Milliseconds = CARRIED_MEDIAN_TICK_MILLISECONDS,
      .tickP95Milliseconds = std::nullopt,
      .receivedBytesPerSecond = std::nullopt,
      .sentBytesPerSecond = std::nullopt,
      .lossPercent = std::nullopt,
      .averagePingMilliseconds = std::nullopt};

  EXPECT_EQ(heartbeatOf({}, measures).measures, measures);
}

/**
 * Given no measures, as before the supervised games published any
 * When a heartbeat is written
 * Then it carries the games and no measure
 */
TEST(MasterMessages, AHeartbeatWithoutMeasuresCarriesNone) {
  const ServerHeartbeat heartbeat = heartbeatOf({}, std::nullopt);

  EXPECT_TRUE(heartbeat.games.empty());
  EXPECT_EQ(heartbeat.measures, ServerMeasures{});
}
