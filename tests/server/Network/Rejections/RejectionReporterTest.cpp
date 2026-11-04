#include <gtest/gtest.h>
#include "RejectionReporterFixture.hpp"
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
