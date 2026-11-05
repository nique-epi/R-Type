#include "RejectionLines.hpp"
#include <cstddef>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include "RejectedSender.hpp"
#include "RejectionCounts.hpp"
#include "RejectionReport.hpp"
#include "RejectionsConstants.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

namespace {

static_assert(!REJECTION_CAUSE_NAMES.back().empty(),
              "every RejectionCause needs a name");

/**
 * @returns The count of each cause of @p counts that is not 0, in the order the
 * causes are declared, such as "1 stranger, 3 wrong session token".
 */
std::string causesTextOf(const RejectionCounts& counts) {
  std::ostringstream text;
  std::string_view separator;
  for (std::size_t cause = 0; cause < REJECTION_CAUSE_COUNT; ++cause) {
    if (counts.at(cause) != 0) {
      text << separator << counts.at(cause) << ' '
           << REJECTION_CAUSE_NAMES.at(cause);
      separator = ", ";
    }
  }
  return text.str();
}

std::string causesLineOf(const RejectionReport& report) {
  std::ostringstream line;
  line << "rejected " << totalOf(report.sincePreviousReport)
       << " datagrams in the last " << REPORT_SECONDS << " s ("
       << report.rejectedSinceStart
       << " since the start): " << causesTextOf(report.sincePreviousReport);
  return line.str();
}

std::string senderLineOf(const RejectedSender& sender) {
  std::ostringstream line;
  line << "rejected " << totalOf(sender.counts) << " datagrams from "
       << sender.endpoint.address << " port " << sender.endpoint.port
       << " in the last " << REPORT_SECONDS
       << " s: " << causesTextOf(sender.counts);
  return line.str();
}

std::string otherSendersLineOf(const RejectionReport& report) {
  std::ostringstream line;
  line << "rejected datagrams from " << report.otherSenders
       << " more senders in the last " << REPORT_SECONDS << " s, besides "
       << report.forgottenSenders << " forgotten to keep at most "
       << REJECTED_SENDER_CAPACITY;
  return line.str();
}

}  // namespace

std::vector<std::string> rejectionLinesOf(const RejectionReport& report) {
  if (totalOf(report.sincePreviousReport) == 0) {
    return {};
  }
  std::vector<std::string> lines{causesLineOf(report)};
  for (const RejectedSender& sender : report.mostRejected) {
    lines.push_back(senderLineOf(sender));
  }
  if (report.otherSenders + report.forgottenSenders != 0) {
    lines.push_back(otherSendersLineOf(report));
  }
  return lines;
}

}  // namespace rtype::server
