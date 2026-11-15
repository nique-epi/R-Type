#include "RejectionEvents.hpp"
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include "EventField.hpp"
#include "RejectedSender.hpp"
#include "RejectionCounts.hpp"
#include "RejectionEvent.hpp"
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

/** @returns A field of @p name and the whole number @p count. */
logging::EventField countField(std::string_view name, std::size_t count) {
  return {.name = name, .value = static_cast<std::int64_t>(count)};
}

/** @brief Adds to @p fields the length of a report, the total of @p counts,
 * then one field for each cause of @p counts that is not 0, named after it. */
void addCountFields(std::vector<logging::EventField>& fields,
                    const RejectionCounts& counts) {
  fields.push_back(countField(SECONDS_FIELD, REPORT_SECONDS));
  fields.push_back(countField(TOTAL_FIELD, totalOf(counts)));
  for (std::size_t cause = 0; cause < REJECTION_CAUSE_COUNT; ++cause) {
    if (counts.at(cause) != 0) {
      fields.push_back(
          countField(REJECTION_CAUSE_NAMES.at(cause), counts.at(cause)));
    }
  }
}

RejectionEvent causesEventOf(const RejectionReport& report) {
  std::ostringstream message;
  message << "rejected " << totalOf(report.sincePreviousReport)
          << " datagrams in the last " << REPORT_SECONDS << " s ("
          << report.rejectedSinceStart
          << " since the start): " << causesTextOf(report.sincePreviousReport);
  RejectionEvent event{
      .name = REJECTED_DATAGRAMS_EVENT, .message = message.str(), .fields = {}};
  addCountFields(event.fields, report.sincePreviousReport);
  event.fields.push_back(
      countField(SINCE_START_FIELD, report.rejectedSinceStart));
  return event;
}

RejectionEvent senderEventOf(const RejectedSender& sender) {
  std::ostringstream message;
  message << "rejected " << totalOf(sender.counts) << " datagrams from "
          << sender.endpoint.address << " port " << sender.endpoint.port
          << " in the last " << REPORT_SECONDS
          << " s: " << causesTextOf(sender.counts);
  RejectionEvent event{.name = REJECTED_FROM_SENDER_EVENT,
                       .message = message.str(),
                       .fields = {}};
  event.fields.push_back({.name = ADDRESS_FIELD,
                          .value = std::string_view{sender.endpoint.address}});
  event.fields.push_back(countField(PORT_FIELD, sender.endpoint.port));
  addCountFields(event.fields, sender.counts);
  return event;
}

RejectionEvent otherSendersEventOf(const RejectionReport& report) {
  std::ostringstream message;
  message << "rejected datagrams from " << report.otherSenders
          << " more senders in the last " << REPORT_SECONDS << " s, besides "
          << report.forgottenSenders << " forgotten to keep at most "
          << REJECTED_SENDER_CAPACITY;
  return RejectionEvent{
      .name = REJECTED_FROM_OTHER_SENDERS_EVENT,
      .message = message.str(),
      .fields = {countField(SECONDS_FIELD, REPORT_SECONDS),
                 countField(SENDERS_FIELD, report.otherSenders),
                 countField(FORGOTTEN_SENDERS_FIELD, report.forgottenSenders)}};
}

}  // namespace

std::vector<RejectionEvent> rejectionEventsOf(const RejectionReport& report) {
  if (totalOf(report.sincePreviousReport) == 0) {
    return {};
  }
  std::vector<RejectionEvent> events{causesEventOf(report)};
  for (const RejectedSender& sender : report.mostRejected) {
    events.push_back(senderEventOf(sender));
  }
  if (report.otherSenders + report.forgottenSenders != 0) {
    events.push_back(otherSendersEventOf(report));
  }
  return events;
}

}  // namespace rtype::server
