#include <gtest/gtest.h>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <vector>
#include "JsonJournal.hpp"
#include "RejectionReporterFixture.hpp"
#include "RejectionsFixtures.hpp"
#include "RejectionsTestConstants.hpp"
#include "ServerConstants.hpp"

using rtype::server::TICKS_PER_REPORT;

/**
 * Given a reporter on a ledger that counted one stranger
 * When one tick less than a report is counted
 * Then the journal holds no line of the report
 */
TEST_F(RejectionReporterFixture, WritesNothingBeforeTheEndOfAReport) {
  countTicks(TICKS_PER_REPORT - 1);

  EXPECT_EQ(reportLineCount(), 0U);
}

/**
 * Given a reporter on a ledger that counted one stranger
 * When the tick that ends a report is counted
 * Then the journal holds the line of the causes and the line of the stranger
 */
TEST_F(RejectionReporterFixture, WritesTheReportAtTheTickThatEndsIt) {
  countTicks(TICKS_PER_REPORT);

  EXPECT_EQ(reportLineCount(), ONE_SENDER_REPORT_LINES);
}

/**
 * Given a reporter that just wrote a report, and one more stranger counted
 * When one tick less than a report is counted again
 * Then the journal still holds only the lines of the first report
 */
TEST_F(RejectionReporterFixture, WaitsAWholeReportAgainAfterOne) {
  countTicks(TICKS_PER_REPORT);
  rejectOneStranger();

  countTicks(TICKS_PER_REPORT - 1);

  EXPECT_EQ(reportLineCount(), ONE_SENDER_REPORT_LINES);
}

/**
 * Given a journal written as JSON Lines and a reporter on a ledger that
 * counted one stranger
 * When the tick that ends a report is counted
 * Then the journal holds one event of the causes, counting the stranger, and
 * one event of its sender, naming its port
 */
TEST_F(RejectionReporterFixture, WritesTheReportAsEvents) {
  const JsonJournal journal;

  countTicks(TICKS_PER_REPORT);

  const std::vector<nlohmann::json> causes =
      journal.file().events(CAUSES_EVENT_NAME);
  ASSERT_EQ(causes.size(), 1U);
  EXPECT_EQ(causes.front().at("stranger").get<std::int64_t>(), 1);
  const std::vector<nlohmann::json> senders =
      journal.file().events(SENDER_EVENT_NAME);
  ASSERT_EQ(senders.size(), 1U);
  EXPECT_EQ(senders.front().at("port").get<std::int64_t>(),
            rejectedSender(0).port);
}
