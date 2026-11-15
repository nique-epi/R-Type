#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "RejectedSender.hpp"
#include "RejectionCause.hpp"
#include "RejectionEvent.hpp"
#include "RejectionEvents.hpp"
#include "RejectionReport.hpp"
#include "RejectionsFixtures.hpp"
#include "RejectionsTestConstants.hpp"

using rtype::network::Endpoint;
using rtype::server::RejectedSender;
using rtype::server::RejectionCause;
using rtype::server::RejectionEvent;
using rtype::server::rejectionEventsOf;
using rtype::server::RejectionReport;

/**
 * Given a report of nothing rejected since the previous one, but some before
 * When its events are written
 * Then there are none: a quiet server writes nothing
 */
TEST(RejectionEvents,
     WritesNothingWhenNothingWasRejectedSinceThePreviousReport) {
  const RejectionReport report{.rejectedSinceStart = REJECTED_SINCE_START,
                               .mostRejected = {}};

  EXPECT_TRUE(rejectionEventsOf(report).empty());
}

/**
 * Given a report of 1 stranger and 3 wrong tokens, 5 since the start, the
 * wrong tokens from one sender it names
 * When its events are written
 * Then the first counts each cause, the second the sender, by address and port
 */
TEST(RejectionEvents, CountsEachCauseThenEachSenderNamed) {
  const RejectionReport report{
      .sincePreviousReport =
          countsOf({RejectionCause::Stranger, RejectionCause::WrongSessionToken,
                    RejectionCause::WrongSessionToken,
                    RejectionCause::WrongSessionToken}),
      .rejectedSinceStart = REJECTED_SINCE_START,
      .mostRejected = {RejectedSender{
          .endpoint = Endpoint{.address = std::string{LINE_SENDER_ADDRESS},
                               .port = LINE_SENDER_PORT},
          .counts = countsOf({RejectionCause::WrongSessionToken,
                              RejectionCause::WrongSessionToken,
                              RejectionCause::WrongSessionToken})}}};

  EXPECT_EQ(messagesOf(rejectionEventsOf(report)),
            (std::vector<std::string>{std::string{CAUSES_LINE},
                                      std::string{SENDER_LINE}}));
}

/**
 * Given a report of one stranger, with 2 senders it does not name and 1
 * forgotten
 * When its events are written
 * Then the last line counts them
 */
TEST(RejectionEvents, CountsTheSendersItDoesNotName) {
  const RejectionReport report{
      .sincePreviousReport = countsOf({RejectionCause::Stranger}),
      .mostRejected = {},
      .otherSenders = UNNAMED_SENDERS,
      .forgottenSenders = FORGOTTEN_SENDERS};

  const std::vector<std::string> lines = messagesOf(rejectionEventsOf(report));

  ASSERT_FALSE(lines.empty());
  EXPECT_EQ(lines.back(), OTHER_SENDERS_LINE);
}

/**
 * Given the report of 1 stranger and 3 wrong tokens, 5 since the start, the
 * wrong tokens from one sender it names
 * When its events are written
 * Then the event of the causes counts the length of the report, the total,
 * each cause by its name and the rejections since the start, and the event of
 * the sender names its address, its port and its causes
 */
TEST(RejectionEvents, NamesTheCountsOfTheReportAsFields) {
  const RejectionReport report{
      .sincePreviousReport =
          countsOf({RejectionCause::Stranger, RejectionCause::WrongSessionToken,
                    RejectionCause::WrongSessionToken,
                    RejectionCause::WrongSessionToken}),
      .rejectedSinceStart = REJECTED_SINCE_START,
      .mostRejected = {RejectedSender{
          .endpoint = Endpoint{.address = std::string{LINE_SENDER_ADDRESS},
                               .port = LINE_SENDER_PORT},
          .counts = countsOf({RejectionCause::WrongSessionToken,
                              RejectionCause::WrongSessionToken,
                              RejectionCause::WrongSessionToken})}}};

  const std::vector<RejectionEvent> events = rejectionEventsOf(report);

  ASSERT_EQ(events.size(), 2U);
  EXPECT_EQ(events.at(0).name, CAUSES_EVENT_NAME);
  EXPECT_EQ(fieldOf(events.at(0), "seconds"), REPORT_LENGTH_FIELD);
  EXPECT_EQ(fieldOf(events.at(0), "total"), CAUSES_TOTAL_FIELD);
  EXPECT_EQ(fieldOf(events.at(0), "stranger"), ONE_FIELD);
  EXPECT_EQ(fieldOf(events.at(0), "wrong session token"), THREE_FIELD);
  EXPECT_EQ(fieldOf(events.at(0), "since_start"), SINCE_START_FIELD_VALUE);
  EXPECT_FALSE(fieldOf(events.at(0), "banned").has_value());
  EXPECT_EQ(events.at(1).name, SENDER_EVENT_NAME);
  EXPECT_EQ(fieldOf(events.at(1), "address"), SENDER_ADDRESS_FIELD);
  EXPECT_EQ(fieldOf(events.at(1), "port"), SENDER_PORT_FIELD);
  EXPECT_EQ(fieldOf(events.at(1), "wrong session token"), THREE_FIELD);
}

/**
 * Given a report of one stranger, with 2 senders it does not name and 1
 * forgotten
 * When its events are written
 * Then the last event counts them as fields
 */
TEST(RejectionEvents, NamesTheSendersItDoesNotNameAsFields) {
  const RejectionReport report{
      .sincePreviousReport = countsOf({RejectionCause::Stranger}),
      .mostRejected = {},
      .otherSenders = UNNAMED_SENDERS,
      .forgottenSenders = FORGOTTEN_SENDERS};

  const std::vector<RejectionEvent> events = rejectionEventsOf(report);

  ASSERT_FALSE(events.empty());
  EXPECT_EQ(events.back().name, OTHER_SENDERS_EVENT_NAME);
  EXPECT_EQ(fieldOf(events.back(), "senders"), UNNAMED_SENDERS_FIELD);
  EXPECT_EQ(fieldOf(events.back(), "forgotten_senders"), ONE_FIELD);
}
