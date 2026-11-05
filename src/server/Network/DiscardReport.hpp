#pragma once

#include <cstddef>
#include "DatagramDrops.hpp"
#include "DatagramQueues.hpp"
#include "Logger.hpp"

namespace rtype::server {

/**
 * @brief Warns, every TICKS_PER_REPORT ticks, of the datagrams the server lost
 * since the previous report: discarded by a full queue, or dropped by the
 * sorter. A report with nothing to say writes nothing.
 *
 * Use it on the simulation thread only.
 */
class DiscardReport {
 public:
  /** @param queues, drops What the report reads; they must outlive it. */
  DiscardReport(DatagramQueues queues, DatagramDrops& drops);

  /** @brief Counts one tick, and reports when it ends a report. */
  void countTick();

 private:
  void report();
  void reportDiscards();
  void reportDrops();

  logging::Logger logger_;
  DatagramQueues queues_;
  DatagramDrops* drops_;
  std::size_t ticksSinceReport_{0};
  std::size_t reportedDiscardCount_{0};
  std::size_t reportedRequestDiscardCount_{0};
  std::size_t reportedInformationDiscardCount_{0};
  std::size_t reportedDroppedCount_{0};
  std::size_t reportedWrongTokenCount_{0};
};

}  // namespace rtype::server
