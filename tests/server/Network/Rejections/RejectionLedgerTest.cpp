#include <gtest/gtest.h>
#include <cstddef>
#include "RejectionCause.hpp"
#include "RejectionCounts.hpp"
#include "RejectionLedger.hpp"
#include "RejectionReport.hpp"
#include "RejectionsConstants.hpp"
#include "RejectionsFixtures.hpp"
#include "RejectionsTestConstants.hpp"

using rtype::server::REJECTED_SENDER_CAPACITY;
using rtype::server::RejectionCause;
using rtype::server::RejectionLedger;
using rtype::server::RejectionReport;
using rtype::server::REPORTED_SENDER_COUNT;
using rtype::server::totalOf;

/**
 * Given a ledger that recorded nothing
 * When its report is taken
 * Then it counts nothing and names no sender
 */
TEST(RejectionLedger, ReportsNothingBeforeTheFirstRejection) {
  RejectionLedger ledger;

  const RejectionReport report = ledger.takeReport();

  EXPECT_EQ(totalOf(report.sincePreviousReport), 0U);
  EXPECT_EQ(report.rejectedSinceStart, 0U);
  EXPECT_TRUE(report.mostRejected.empty());
  EXPECT_EQ(report.otherSenders, 0U);
  EXPECT_EQ(report.forgottenSenders, 0U);
}

/**
 * Given a ledger
 * When two wrong tokens come from one sender, then a stranger's datagram from
 * another
 * Then the report counts each cause, and each sender with its own causes
 */
TEST(RejectionLedger, CountsEachCauseForEachSender) {
  RejectionLedger ledger;

  ledger.record(RejectionCause::WrongSessionToken, rejectedSender(0));
  ledger.record(RejectionCause::WrongSessionToken, rejectedSender(0));
  ledger.record(RejectionCause::Stranger, rejectedSender(1));

  const RejectionReport report = ledger.takeReport();
  EXPECT_EQ(
      report.sincePreviousReport,
      countsOf({RejectionCause::WrongSessionToken,
                RejectionCause::WrongSessionToken, RejectionCause::Stranger}));
  ASSERT_EQ(report.mostRejected.size(), 2U);
  EXPECT_EQ(report.mostRejected[0].endpoint, rejectedSender(0));
  EXPECT_EQ(report.mostRejected[0].counts,
            countsOf({RejectionCause::WrongSessionToken,
                      RejectionCause::WrongSessionToken}));
  EXPECT_EQ(report.mostRejected[1].endpoint, rejectedSender(1));
  EXPECT_EQ(report.mostRejected[1].counts,
            countsOf({RejectionCause::Stranger}));
}

/**
 * Given a ledger whose report was taken after one rejection
 * When one more datagram is rejected and the report is taken again
 * Then the count since the start goes on, while the counts of each cause
 * and those of the sender start afresh
 */
TEST(RejectionLedger, StartsTheSendersAfreshAtEveryReport) {
  RejectionLedger ledger;
  ledger.record(RejectionCause::Stranger, rejectedSender(0));
  static_cast<void>(ledger.takeReport());

  ledger.record(RejectionCause::Stranger, rejectedSender(0));

  const RejectionReport report = ledger.takeReport();
  EXPECT_EQ(report.rejectedSinceStart, 2U);
  EXPECT_EQ(report.sincePreviousReport, countsOf({RejectionCause::Stranger}));
  ASSERT_EQ(report.mostRejected.size(), 1U);
  EXPECT_EQ(totalOf(report.mostRejected[0].counts), 1U);
}

/**
 * Given a ledger
 * When one more sender than a report names is rejected, each one more often
 * than the one before
 * Then the report names the most rejected, most first, and counts the least
 * rejected as another sender
 */
TEST(RejectionLedger, NamesTheMostRejectedSendersFirst) {
  RejectionLedger ledger;

  for (std::size_t sender = 0; sender <= REPORTED_SENDER_COUNT; ++sender) {
    for (std::size_t rejection = 0; rejection <= sender; ++rejection) {
      ledger.record(RejectionCause::Stranger, rejectedSender(sender));
    }
  }

  const RejectionReport report = ledger.takeReport();
  ASSERT_EQ(report.mostRejected.size(), REPORTED_SENDER_COUNT);
  for (std::size_t place = 0; place < REPORTED_SENDER_COUNT; ++place) {
    EXPECT_EQ(report.mostRejected[place].endpoint,
              rejectedSender(REPORTED_SENDER_COUNT - place));
  }
  EXPECT_EQ(report.otherSenders, 1U);
}

/**
 * Given a ledger
 * When two senders are rejected once each
 * Then the one rejected last is named first
 */
TEST(RejectionLedger, NamesTheMostRecentFirstAmongSendersRejectedAsOften) {
  RejectionLedger ledger;

  recordStrangers(ledger, 2);

  const RejectionReport report = ledger.takeReport();
  ASSERT_EQ(report.mostRejected.size(), 2U);
  EXPECT_EQ(report.mostRejected[0].endpoint, rejectedSender(1));
  EXPECT_EQ(report.mostRejected[1].endpoint, rejectedSender(0));
}

/**
 * Given a ledger
 * When exactly as many senders as it keeps are rejected
 * Then it forgets none of them
 */
TEST(RejectionLedger, KeepsAsManySendersAsItsRoom) {
  RejectionLedger ledger;

  recordStrangers(ledger, REJECTED_SENDER_CAPACITY);

  const RejectionReport report = ledger.takeReport();
  EXPECT_EQ(report.forgottenSenders, 0U);
  EXPECT_EQ(report.mostRejected.size() + report.otherSenders,
            REJECTED_SENDER_CAPACITY);
}

/**
 * Given a full ledger whose first sender was rejected again after the others
 * When a new sender is rejected, then the second sender three times
 * Then the second sender had been forgotten for the new one and counts its
 * three rejections only, and the first, rejected recently, kept its two
 */
TEST(RejectionLedger, ForgetsTheSenderRejectedLeastRecently) {
  RejectionLedger ledger;
  recordStrangers(ledger, REJECTED_SENDER_CAPACITY);
  recordStranger(ledger, 0);

  recordStranger(ledger, REJECTED_SENDER_CAPACITY);
  for (std::size_t rejection = 0; rejection < RETURNING_SENDER_REJECTIONS;
       ++rejection) {
    recordStranger(ledger, 1);
  }

  const RejectionReport report = ledger.takeReport();
  ASSERT_GE(report.mostRejected.size(), 2U);
  EXPECT_EQ(report.mostRejected[0].endpoint, rejectedSender(1));
  EXPECT_EQ(totalOf(report.mostRejected[0].counts),
            RETURNING_SENDER_REJECTIONS);
  EXPECT_EQ(report.mostRejected[1].endpoint, rejectedSender(0));
  EXPECT_EQ(totalOf(report.mostRejected[1].counts), 2U);
  EXPECT_EQ(report.forgottenSenders, 2U);
}
