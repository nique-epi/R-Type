#include "LatestStatusMeasures.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <ratio>
#include "DurationConversion.hpp"
#include "GameStatusMeasures.hpp"
#include "LatestMetrics.hpp"
#include "MetricsReport.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @returns @p duration in whole microseconds, rounded to the nearest and kept
 * within the 32 bits of the wire. */
std::uint32_t microsecondsOf(engine::Duration duration) {
  const double microseconds =
      std::chrono::duration<double, std::micro>(duration).count();
  return static_cast<std::uint32_t>(std::clamp(
      std::round(microseconds), 0.0,
      static_cast<double>(std::numeric_limits<std::uint32_t>::max())));
}

/** @returns The mean round trip of the players of @p report; nothing when it
 * measured none. */
std::optional<std::uint32_t> averageRoundTripOf(const MetricsReport& report) {
  if (report.roundTrips.empty()) {
    return std::nullopt;
  }
  engine::Duration total{0};
  for (const PlayerRoundTripMeasure& player : report.roundTrips) {
    total += player.figures.roundTrip;
  }
  return microsecondsOf(engine::averageOf(total, report.roundTrips.size()));
}

/** @returns The measures of @p report a status carries; a measure the report
 * has not taken stays empty. */
network::GameStatusMeasures statusMeasuresOf(const MetricsReport& report) {
  network::GameStatusMeasures measures{};
  if (report.ticks.count > 0) {
    measures.tickMedianMicroseconds = microsecondsOf(report.ticks.median);
    measures.tickNinetyFifthPercentileMicroseconds =
        microsecondsOf(report.ticks.ninetyFifthPercentile);
  }
  measures.averageRoundTripMicroseconds = averageRoundTripOf(report);
  return measures;
}

}  // namespace

LatestStatusMeasures::LatestStatusMeasures(const LatestMetrics* metrics)
    : metrics_(metrics) {}

const network::GameStatusMeasures& LatestStatusMeasures::current() {
  if (metrics_ == nullptr || metrics_->revision() == measuredRevision_) {
    return measures_;
  }
  measuredRevision_ = metrics_->revision();
  const std::optional<MetricsReport> report = metrics_->value();
  measures_ = report.has_value() ? statusMeasuresOf(*report)
                                 : network::GameStatusMeasures{};
  return measures_;
}

}  // namespace rtype::server
