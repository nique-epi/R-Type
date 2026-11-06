#include <gtest/gtest.h>
#include "GameStatusMeasures.hpp"
#include "LatestMetrics.hpp"
#include "LatestStatusMeasures.hpp"
#include "LatestStatusMeasuresTestConstants.hpp"
#include "MetricsReport.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "TickDurationSummary.hpp"

using rtype::network::GameStatusMeasures;
using rtype::server::LatestMetrics;
using rtype::server::LatestStatusMeasures;
using rtype::server::MetricsReport;
using rtype::server::PlayerRoundTripMeasure;
using rtype::server::TickDurationSummary;
using rtype::server::testing::FIVE_MILLISECOND_SLOW_TICK;
using rtype::server::testing::FIVE_MILLISECONDS_IN_MICROSECONDS;
using rtype::server::testing::FORTY_MILLISECOND_ROUND_TRIP;
using rtype::server::testing::SEVEN_MILLISECOND_ROUND_TRIP;
using rtype::server::testing::SEVEN_MILLISECONDS_IN_MICROSECONDS;
using rtype::server::testing::THIRTY_MILLISECONDS_IN_MICROSECONDS;
using rtype::server::testing::TICKS_OF_TEN_SECONDS;
using rtype::server::testing::TWENTY_MILLISECOND_ROUND_TRIP;
using rtype::server::testing::TWO_MILLISECOND_MEDIAN_TICK;
using rtype::server::testing::TWO_MILLISECONDS_IN_MICROSECONDS;

/**
 * Given a published report of ticks of 2 ms at the median and 5 ms at the 95th
 * percentile, and players 20 ms and 40 ms away
 * When the measures of a status are read
 * Then they are 2000 microseconds, 5000 microseconds and a mean round trip of
 * 30000 microseconds
 */
TEST(LatestStatusMeasures, TheMeasuresComeFromTheReport) {
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
  LatestMetrics metrics;
  metrics.publish(report);
  LatestStatusMeasures measures{&metrics};

  EXPECT_EQ(measures.current(),
            (GameStatusMeasures{
                .tickMedianMicroseconds = TWO_MILLISECONDS_IN_MICROSECONDS,
                .tickNinetyFifthPercentileMicroseconds =
                    FIVE_MILLISECONDS_IN_MICROSECONDS,
                .averageRoundTripMicroseconds =
                    THIRTY_MILLISECONDS_IN_MICROSECONDS}));
}

/**
 * Given a published report that measured no tick and no player
 * When the measures of a status are read
 * Then each is left out, never sent as 0
 */
TEST(LatestStatusMeasures, AMeasureNotTakenIsLeftOut) {
  LatestMetrics metrics;
  metrics.publish(MetricsReport{});
  LatestStatusMeasures measures{&metrics};

  EXPECT_EQ(measures.current(), GameStatusMeasures{});
}

/**
 * Given no metrics module
 * When the measures of a status are read
 * Then each is left out
 */
TEST(LatestStatusMeasures, WithoutMetricsNothingIsMeasured) {
  LatestStatusMeasures measures{nullptr};

  EXPECT_EQ(measures.current(), GameStatusMeasures{});
}

/**
 * Given measures read from a first report
 * When a newer report is published, with a player 7 ms away
 * Then the measures read next follow the newer report
 */
TEST(LatestStatusMeasures, ANewerReportReplacesTheMeasures) {
  LatestMetrics metrics;
  metrics.publish(MetricsReport{});
  LatestStatusMeasures measures{&metrics};
  static_cast<void>(measures.current());
  MetricsReport newer{};
  newer.roundTrips = {PlayerRoundTripMeasure{
      .nickname = {}, .figures = {.roundTrip = SEVEN_MILLISECOND_ROUND_TRIP}}};

  metrics.publish(newer);

  EXPECT_EQ(measures.current().averageRoundTripMicroseconds,
            SEVEN_MILLISECONDS_IN_MICROSECONDS);
}
