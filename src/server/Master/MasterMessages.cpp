#include "MasterMessages.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "DurationConversion.hpp"
#include "HostedGame.hpp"
#include "InformationConstants.hpp"
#include "MetricsReport.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "ProtocolConstants.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerIdentity.hpp"
#include "ServerMasterConstants.hpp"
#include "ServerMeasures.hpp"
#include "ServerRegistration.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

std::optional<double> averagePingOf(const MetricsReport& report) {
  if (report.roundTrips.empty()) {
    return std::nullopt;
  }
  engine::Duration total{0};
  for (const PlayerRoundTripMeasure& player : report.roundTrips) {
    total += player.figures.roundTrip;
  }
  return engine::inMilliseconds(
      engine::averageOf(total, report.roundTrips.size()));
}

}  // namespace

network::ServerRegistration registrationOf(const ServerIdentity& identity) {
  return network::ServerRegistration{
      .name = identity.name,
      .address = std::nullopt,
      .udpPort = identity.udpPort,
      .gameVersion = std::string{SERVER_GAME_VERSION},
      .protocolVersion = network::PROTOCOL_VERSION,
      .maximumGames = identity.maximumGames,
      .maximumPlayersPerGame =
          static_cast<std::uint8_t>(network::MAXIMUM_PLAYERS)};
}

network::ServerMeasures measuresOf(const MetricsReport& report) {
  network::ServerMeasures measures{};
  if (report.ticks.count > 0) {
    measures.tickP50Milliseconds = engine::inMilliseconds(report.ticks.median);
    measures.tickP95Milliseconds =
        engine::inMilliseconds(report.ticks.ninetyFifthPercentile);
  }
  measures.averagePingMilliseconds = averagePingOf(report);
  return measures;
}

network::ServerHeartbeat heartbeatOf(
    std::vector<network::HostedGame> games,
    const std::optional<MetricsReport>& report) {
  return network::ServerHeartbeat{.games = std::move(games),
                                  .measures = report.has_value()
                                                  ? measuresOf(*report)
                                                  : network::ServerMeasures{}};
}

}  // namespace rtype::server
