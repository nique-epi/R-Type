#include <gtest/gtest.h>
#include <cstddef>
#include "DebugJournal.hpp"
#include "RejectionCause.hpp"
#include "RejectionLedger.hpp"
#include "RejectionReporter.hpp"
#include "RejectionsFixtures.hpp"
#include "RejectionsTestConstants.hpp"
#include "ServerConstants.hpp"

using rtype::server::RejectionCause;
using rtype::server::RejectionLedger;
using rtype::server::RejectionReporter;
using rtype::server::TICKS_PER_REPORT;

/**
 * Given a reporter on a ledger that counted one stranger
 * When one tick less than a report is counted
 * Then the journal holds no line of the report
 */
TEST(RejectionReporter, WritesNothingBeforeTheEndOfAReport) {
  const DebugJournal journal;
  RejectionLedger ledger;
  RejectionReporter reporter{ledger};
  ledger.record(RejectionCause::Stranger, rejectedSender(0));

  for (std::size_t tick = 1; tick < TICKS_PER_REPORT; ++tick) {
    reporter.countTick();
  }

  EXPECT_TRUE(journal.linesHolding(REPORT_LINE_TEXT).empty());
}

/**
 * Given a reporter on a ledger that counted one stranger
 * When the tick that ends a report is counted
 * Then the journal holds the line of the causes and the line of the stranger
 */
TEST(RejectionReporter, WritesTheReportAtTheTickThatEndsIt) {
  const DebugJournal journal;
  RejectionLedger ledger;
  RejectionReporter reporter{ledger};
  ledger.record(RejectionCause::Stranger, rejectedSender(0));

  for (std::size_t tick = 1; tick <= TICKS_PER_REPORT; ++tick) {
    reporter.countTick();
  }

  EXPECT_EQ(journal.linesHolding(REPORT_LINE_TEXT).size(), 2U);
}

/**
 * Given a reporter that just wrote a report, and one more stranger counted
 * When one tick less than a report is counted again
 * Then the journal still holds only the two lines of the first report
 */
TEST(RejectionReporter, WaitsAWholeReportAgainAfterOne) {
  const DebugJournal journal;
  RejectionLedger ledger;
  RejectionReporter reporter{ledger};
  ledger.record(RejectionCause::Stranger, rejectedSender(0));
  for (std::size_t tick = 1; tick <= TICKS_PER_REPORT; ++tick) {
    reporter.countTick();
  }
  ledger.record(RejectionCause::Stranger, rejectedSender(0));

  for (std::size_t tick = 1; tick < TICKS_PER_REPORT; ++tick) {
    reporter.countTick();
  }

  EXPECT_EQ(journal.linesHolding(REPORT_LINE_TEXT).size(), 2U);
}
