#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "RejectedSender.hpp"
#include "RejectionCause.hpp"
#include "RejectionLines.hpp"
#include "RejectionReport.hpp"
#include "RejectionsFixtures.hpp"
#include "RejectionsTestConstants.hpp"

using rtype::network::Endpoint;
using rtype::server::RejectedSender;
using rtype::server::RejectionCause;
using rtype::server::rejectionLinesOf;
using rtype::server::RejectionReport;

/**
 * Given a report of nothing rejected since the previous one, but some before
 * When its lines are written
 * Then there are none: a quiet server writes nothing
 */
TEST(RejectionLines,
     WritesNothingWhenNothingWasRejectedSinceThePreviousReport) {
  const RejectionReport report{
      .sinceStart = countsOf({RejectionCause::Stranger}), .mostRejected = {}};

  EXPECT_TRUE(rejectionLinesOf(report).empty());
}

/**
 * Given a report of 1 stranger and 3 wrong tokens, 5 since the start, the
 * wrong tokens from one sender it names
 * When its lines are written
 * Then the first counts each cause, the second the sender, by address and port
 */
TEST(RejectionLines, CountsEachCauseThenEachSenderNamed) {
  const RejectionReport report{
      .sincePreviousReport =
          countsOf({RejectionCause::Stranger, RejectionCause::WrongSessionToken,
                    RejectionCause::WrongSessionToken,
                    RejectionCause::WrongSessionToken}),
      .sinceStart = countsOf(
          {RejectionCause::Stranger, RejectionCause::Stranger,
           RejectionCause::WrongSessionToken, RejectionCause::WrongSessionToken,
           RejectionCause::WrongSessionToken}),
      .mostRejected = {RejectedSender{
          .endpoint = Endpoint{.address = std::string{LINE_SENDER_ADDRESS},
                               .port = LINE_SENDER_PORT},
          .counts = countsOf({RejectionCause::WrongSessionToken,
                              RejectionCause::WrongSessionToken,
                              RejectionCause::WrongSessionToken})}}};

  EXPECT_EQ(rejectionLinesOf(report),
            (std::vector<std::string>{std::string{CAUSES_LINE},
                                      std::string{SENDER_LINE}}));
}

/**
 * Given a report of one stranger, with 2 senders it does not name and 1
 * forgotten
 * When its lines are written
 * Then the last line counts them
 */
TEST(RejectionLines, CountsTheSendersItDoesNotName) {
  const RejectionReport report{
      .sincePreviousReport = countsOf({RejectionCause::Stranger}),
      .mostRejected = {},
      .otherSenders = 2,
      .forgottenSenders = 1};

  const std::vector<std::string> lines = rejectionLinesOf(report);

  ASSERT_FALSE(lines.empty());
  EXPECT_EQ(lines.back(), OTHER_SENDERS_LINE);
}
