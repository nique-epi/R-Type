#include "RejectionReporter.hpp"
#include <string>
#include "RejectionLedger.hpp"
#include "RejectionLines.hpp"
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
  for (const std::string& line : rejectionLinesOf(rejections_->takeReport())) {
    logger_.warn(line);
  }
}

}  // namespace rtype::server
