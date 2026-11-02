#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <thread>
#include "LatestMetrics.hpp"
#include "LatestMetricsFixtures.hpp"
#include "LatestMetricsTestConstants.hpp"
#include "MetricsReport.hpp"

namespace rtype::server {

using testing::MOST_SYSTEM_TIMES;
using testing::PLAYERS_OF_THE_NEWER_REPORT;
using testing::PLAYERS_OF_THE_OLDER_REPORT;
using testing::reportCounting;
using testing::REPORTS_PUBLISHED;
using testing::TIMES_READ;

/**
 * Given nothing was published
 * When the report is read
 * Then there is none
 */
TEST(LatestMetrics, HoldsNoReportBeforeTheFirstOne) {
  const LatestMetrics latest;

  EXPECT_FALSE(latest.report().has_value());
}

/**
 * Given a report was published
 * When the report is read
 * Then it is that report
 */
TEST(LatestMetrics, GivesTheReportPublished) {
  LatestMetrics latest;
  latest.publish(reportCounting(PLAYERS_OF_THE_OLDER_REPORT));

  const std::optional<MetricsReport> report = latest.report();

  ASSERT_TRUE(report.has_value());
  EXPECT_EQ(report.value_or(MetricsReport{}).games.players,
            PLAYERS_OF_THE_OLDER_REPORT);
}

/**
 * Given two reports were published, one after the other
 * When the report is read
 * Then it is the newer one
 */
TEST(LatestMetrics, TheNewerReportReplacesTheOlderOne) {
  LatestMetrics latest;
  latest.publish(reportCounting(PLAYERS_OF_THE_OLDER_REPORT));
  latest.publish(reportCounting(PLAYERS_OF_THE_NEWER_REPORT));

  const std::optional<MetricsReport> report = latest.report();

  ASSERT_TRUE(report.has_value());
  EXPECT_EQ(report.value_or(MetricsReport{}).games.players,
            PLAYERS_OF_THE_NEWER_REPORT);
}

/**
 * Given a report was published
 * When it is read twice, as two readers at different rates do
 * Then the second read finds the same report
 */
TEST(LatestMetrics, ReadingLeavesTheReportInPlace) {
  LatestMetrics latest;
  latest.publish(reportCounting(PLAYERS_OF_THE_OLDER_REPORT));

  [[maybe_unused]] const std::optional<MetricsReport> first = latest.report();
  const std::optional<MetricsReport> second = latest.report();

  ASSERT_TRUE(second.has_value());
  EXPECT_EQ(second.value_or(MetricsReport{}).games.players,
            PLAYERS_OF_THE_OLDER_REPORT);
}

/**
 * Given a thread that keeps publishing reports
 * When another thread reads meanwhile
 * Then each report it reads is whole: all its figures come from one publication
 */
TEST(LatestMetrics, AReaderOnAnotherThreadOnlySeesWholeReports) {
  LatestMetrics latest;
  const std::jthread writer{[&latest] {
    for (std::size_t count = 0; count < REPORTS_PUBLISHED; ++count) {
      latest.publish(reportCounting(count));
    }
  }};

  for (std::size_t read = 0; read < TIMES_READ; ++read) {
    const std::optional<MetricsReport> report = latest.report();
    if (report.has_value()) {
      ASSERT_EQ(report->games.players, report->games.entities);
      ASSERT_EQ(report->systemTimes.size(),
                report->games.players % MOST_SYSTEM_TIMES);
    }
  }
}

}  // namespace rtype::server
