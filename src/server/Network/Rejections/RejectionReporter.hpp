#pragma once

#include <cstddef>
#include "Logger.hpp"
#include "RejectionLedger.hpp"

namespace rtype::server {

/**
 * @brief Writes, every TICKS_PER_REPORT ticks, the lines of the datagrams the
 * RejectionLedger counted since the previous report, at the warn level (see
 * rejectionLinesOf()). A report with nothing to say writes nothing.
 *
 * Use it on the simulation thread only; the ledger may be fed from any thread.
 */
class RejectionReporter {
 public:
  /** @param rejections What the reporter reads; it must outlive it. */
  explicit RejectionReporter(RejectionLedger& rejections);

  /** @brief Counts one tick, and reports when it ends a report. */
  void countTick();

 private:
  logging::Logger logger_;
  RejectionLedger* rejections_;
  std::size_t ticksSinceReport_{0};
};

}  // namespace rtype::server
