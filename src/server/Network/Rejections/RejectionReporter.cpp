#include "RejectionReporter.hpp"
#include <string>
#include "LogEvent.hpp"
#include "LogLevel.hpp"
#include "RejectionEvent.hpp"
#include "RejectionEvents.hpp"
#include "RejectionLedger.hpp"
#include "RejectionReport.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

RejectionReporter::RejectionReporter(RejectionLedger& rejections)
    : logger_(std::string{SERVER_LOGGER_NAME}), rejections_(&rejections) {}

void RejectionReporter::countTick() {
  ++ticksSinceReport_;
  if (ticksSinceReport_ < TICKS_PER_REPORT) {
    return;
  }
  ticksSinceReport_ = 0;
  const RejectionReport report = rejections_->takeReport();
  for (const RejectionEvent& event : rejectionEventsOf(report)) {
    logger_.event(logging::LogEvent{.level = logging::LogLevel::Warn,
                                    .name = event.name,
                                    .message = event.message,
                                    .fields = event.fields});
  }
}

}  // namespace rtype::server
