#include "RejectionReporterFixture.hpp"
#include <cstddef>
#include "RejectionsFixtures.hpp"
#include "RejectionsTestConstants.hpp"

RejectionReporterFixture::RejectionReporterFixture() : reporter_(ledger_) {
  rejectOneStranger();
}

void RejectionReporterFixture::countTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    reporter_.countTick();
  }
}

void RejectionReporterFixture::rejectOneStranger() {
  recordStranger(ledger_, 0);
}

std::size_t RejectionReporterFixture::reportLineCount() const {
  return journal_.linesHolding(REPORT_LINE_TEXT).size();
}
