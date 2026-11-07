#include <gtest/gtest.h>
#include <cstdint>
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
using rtype::server::randomProbeSecret;
using rtype::server::registrationOf;
using rtype::server::ServerIdentity;
using rtype::server::testing::ANNOUNCED_PLAYERS_PER_GAME;
using rtype::server::testing::ANNOUNCED_PROTOCOL_VERSION;
using rtype::server::testing::CARRIED_MEDIAN_TICK_MILLISECONDS;
using rtype::server::testing::LINKED_SERVER_GAMES;
using rtype::server::testing::LINKED_SERVER_NAME;
using rtype::server::testing::LINKED_SERVER_PORT;
using rtype::server::testing::LINKED_SERVER_PROBE_SECRET;
using rtype::server::testing::LINKED_SERVER_PUBLIC_ADDRESS;
using rtype::server::testing::PROBE_SECRET_CEILING;
using rtype::server::testing::PROBE_SECRET_FLOOR;
using rtype::server::testing::PROBE_TOKEN_OF_A_HEARTBEAT;

/**
 * Given a server named, on a port, hosting eight games
 * When its registration is written
 * Then it carries them, protocol 12, four players a game and no address
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
 * Given a server that declares the address players reach it at
 * When its registration is written
 * Then the registration carries that address
 */
TEST(MasterMessages, TheRegistrationCarriesTheDeclaredAddress) {
  const ServerRegistration registration = registrationOf(
      ServerIdentity{.name = std::string{LINKED_SERVER_NAME},
                     .udpPort = LINKED_SERVER_PORT,
                     .maximumGames = LINKED_SERVER_GAMES,
                     .address = std::string{LINKED_SERVER_PUBLIC_ADDRESS}});

  EXPECT_EQ(registration.address.value_or(std::string{}),
            LINKED_SERVER_PUBLIC_ADDRESS);
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

  EXPECT_EQ(heartbeatOf({}, measures, std::nullopt).measures, measures);
}

/**
 * Given no measures, as before the supervised games published any
 * When a heartbeat is written
 * Then it carries the games and no measure
 */
TEST(MasterMessages, AHeartbeatWithoutMeasuresCarriesNone) {
  const ServerHeartbeat heartbeat = heartbeatOf({}, std::nullopt, std::nullopt);

  EXPECT_TRUE(heartbeat.games.empty());
  EXPECT_EQ(heartbeat.measures, ServerMeasures{});
}

/**
 * Given an identity that drew its probe secret
 * When its registration is written
 * Then it carries that secret
 */
TEST(MasterMessages, TheRegistrationCarriesTheProbeSecret) {
  const ServerRegistration registration =
      registrationOf(ServerIdentity{.name = std::string{LINKED_SERVER_NAME},
                                    .udpPort = LINKED_SERVER_PORT,
                                    .maximumGames = LINKED_SERVER_GAMES,
                                    .probeSecret = LINKED_SERVER_PROBE_SECRET});

  EXPECT_EQ(registration.probeSecret, LINKED_SERVER_PROBE_SECRET);
}

/**
 * Given a secret drawn for a registration
 * When it is read
 * Then it lies within the bounds the master accepts
 */
TEST(MasterMessages, ADrawnSecretLiesWithinItsBounds) {
  const std::uint64_t secret = randomProbeSecret();

  EXPECT_GE(secret, PROBE_SECRET_FLOOR);
  EXPECT_LE(secret, PROBE_SECRET_CEILING);
}

/**
 * Given a token of a probe that came since the last heartbeat
 * When a heartbeat is written
 * Then it carries that token
 */
TEST(MasterMessages, AHeartbeatCarriesTheTokenItIsGiven) {
  const ServerHeartbeat heartbeat =
      heartbeatOf({}, std::nullopt, PROBE_TOKEN_OF_A_HEARTBEAT);

  EXPECT_EQ(heartbeat.probeToken, PROBE_TOKEN_OF_A_HEARTBEAT);
}
