#include "MetricsMeasures.hpp"
#include <optional>
#include "DurationConversion.hpp"
#include "MetricsReport.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "ServerMeasures.hpp"
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

}  // namespace rtype::server
