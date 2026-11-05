#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "MasterMessages.hpp"
#include "MasterMessagesTestConstants.hpp"
#include "MetricsReport.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerIdentity.hpp"
#include "ServerMeasures.hpp"
#include "ServerRegistration.hpp"
#include "TickDurationSummary.hpp"

using rtype::network::ServerHeartbeat;
using rtype::network::ServerMeasures;
using rtype::network::ServerRegistration;
using rtype::server::heartbeatOf;
using rtype::server::measuresOf;
using rtype::server::MetricsReport;
using rtype::server::PlayerRoundTripMeasure;
using rtype::server::registrationOf;
using rtype::server::ServerIdentity;
using rtype::server::TickDurationSummary;
using rtype::server::testing::ANNOUNCED_PLAYERS_PER_GAME;
using rtype::server::testing::ANNOUNCED_PROTOCOL_VERSION;
using rtype::server::testing::AVERAGE_PING_MILLISECONDS;
using rtype::server::testing::FAR_PLAYER_ROUND_TRIP;
using rtype::server::testing::LINKED_SERVER_GAMES;
using rtype::server::testing::LINKED_SERVER_NAME;
using rtype::server::testing::LINKED_SERVER_PORT;
using rtype::server::testing::MEASURED_TICKS;
using rtype::server::testing::MEDIAN_TICK;
using rtype::server::testing::MEDIAN_TICK_MILLISECONDS;
using rtype::server::testing::NEAR_PLAYER_ROUND_TRIP;
using rtype::server::testing::SLOW_TICK;
using rtype::server::testing::SLOW_TICK_MILLISECONDS;

/**
 * Given a server named, on a port, hosting eight games
 * When its registration is written
 * Then it carries them, protocol 5, four players a game and no address
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
 * Given a report of ticks of 2 ms at the median and 5 ms at the 95th
 * percentile, and players 20 ms and 40 ms away
 * When its measures are written
 * Then they are 2 ms, 5 ms and a mean round trip of 30 ms
 */
TEST(MasterMessages, TheMeasuresComeFromTheReport) {
  MetricsReport report{};
  report.ticks = TickDurationSummary{.count = MEASURED_TICKS,
                                     .average = MEDIAN_TICK,
                                     .median = MEDIAN_TICK,
                                     .ninetyFifthPercentile = SLOW_TICK,
                                     .worst = SLOW_TICK};
  report.roundTrips = {
      PlayerRoundTripMeasure{.nickname = {},
                             .figures = {.roundTrip = NEAR_PLAYER_ROUND_TRIP}},
      PlayerRoundTripMeasure{.nickname = {},
                             .figures = {.roundTrip = FAR_PLAYER_ROUND_TRIP}}};

  const ServerMeasures measures = measuresOf(report);

  EXPECT_DOUBLE_EQ(measures.tickP50Milliseconds.value_or(0.0),
                   MEDIAN_TICK_MILLISECONDS);
  EXPECT_DOUBLE_EQ(measures.tickP95Milliseconds.value_or(0.0),
                   SLOW_TICK_MILLISECONDS);
  EXPECT_DOUBLE_EQ(measures.averagePingMilliseconds.value_or(0.0),
                   AVERAGE_PING_MILLISECONDS);
}

/**
 * Given a report that measured no tick and no player
 * When its measures are written
 * Then each is left out, never sent as 0
 */
TEST(MasterMessages, AMeasureNotTakenIsLeftOut) {
  EXPECT_EQ(measuresOf(MetricsReport{}), ServerMeasures{});
}

/**
 * Given no report, as when the metrics module is off
 * When a heartbeat is written
 * Then it carries the games and no measure
 */
TEST(MasterMessages, AHeartbeatWithoutReportCarriesNoMeasure) {
  const ServerHeartbeat heartbeat = heartbeatOf({}, std::nullopt);

  EXPECT_TRUE(heartbeat.games.empty());
  EXPECT_EQ(heartbeat.measures, ServerMeasures{});
}
