#include <gtest/gtest.h>
#include "MetricsMeasures.hpp"
#include "MetricsMeasuresTestConstants.hpp"
#include "MetricsReport.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "ServerMeasures.hpp"
#include "TickDurationSummary.hpp"

using rtype::network::ServerMeasures;
using rtype::server::measuresOf;
using rtype::server::MetricsReport;
using rtype::server::PlayerRoundTripMeasure;
using rtype::server::TickDurationSummary;
using rtype::server::testing::FIVE_MILLISECOND_SLOW_TICK;
using rtype::server::testing::FIVE_MILLISECONDS;
using rtype::server::testing::FORTY_MILLISECOND_ROUND_TRIP;
using rtype::server::testing::THIRTY_MILLISECONDS;
using rtype::server::testing::TICKS_OF_TEN_SECONDS;
using rtype::server::testing::TWENTY_MILLISECOND_ROUND_TRIP;
using rtype::server::testing::TWO_MILLISECOND_MEDIAN_TICK;
using rtype::server::testing::TWO_MILLISECONDS;

/**
 * Given a report of ticks of 2 ms at the median and 5 ms at the 95th
 * percentile, and players 20 ms and 40 ms away
 * When its measures are written
 * Then they are 2 ms, 5 ms and a mean round trip of 30 ms
 */
TEST(MetricsMeasures, TheMeasuresComeFromTheReport) {
  MetricsReport report{};
  report.ticks =
      TickDurationSummary{.count = TICKS_OF_TEN_SECONDS,
                          .average = TWO_MILLISECOND_MEDIAN_TICK,
                          .median = TWO_MILLISECOND_MEDIAN_TICK,
                          .ninetyFifthPercentile = FIVE_MILLISECOND_SLOW_TICK,
                          .worst = FIVE_MILLISECOND_SLOW_TICK};
  report.roundTrips = {
      PlayerRoundTripMeasure{
          .nickname = {},
          .figures = {.roundTrip = TWENTY_MILLISECOND_ROUND_TRIP}},
      PlayerRoundTripMeasure{
          .nickname = {},
          .figures = {.roundTrip = FORTY_MILLISECOND_ROUND_TRIP}}};

  const ServerMeasures measures = measuresOf(report);

  EXPECT_DOUBLE_EQ(measures.tickP50Milliseconds.value_or(0.0),
                   TWO_MILLISECONDS);
  EXPECT_DOUBLE_EQ(measures.tickP95Milliseconds.value_or(0.0),
                   FIVE_MILLISECONDS);
  EXPECT_DOUBLE_EQ(measures.averagePingMilliseconds.value_or(0.0),
                   THIRTY_MILLISECONDS);
}

/**
 * Given a report that measured no tick and no player
 * When its measures are written
 * Then each is left out, never sent as 0
 */
TEST(MetricsMeasures, AMeasureNotTakenIsLeftOut) {
  EXPECT_EQ(measuresOf(MetricsReport{}), ServerMeasures{});
}
