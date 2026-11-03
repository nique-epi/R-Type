#pragma once

#include <cstddef>
#include "BoundedQueue.hpp"
#include "DatagramSorter.hpp"
#include "IncomingDatagram.hpp"
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
  /** @param players, connectionRequests, sorter What the report reads; they
   * must outlive it. */
  DiscardReport(
      const concurrency::BoundedQueue<network::IncomingDatagram>& players,
      const concurrency::BoundedQueue<network::IncomingDatagram>&
          connectionRequests,
      DatagramSorter& sorter);

  /** @brief Counts one tick, and reports when it ends a report. */
  void countTick();

 private:
  void report();

  logging::Logger logger_;
  const concurrency::BoundedQueue<network::IncomingDatagram>* players_;
  const concurrency::BoundedQueue<network::IncomingDatagram>*
      connectionRequests_;
  DatagramSorter* sorter_;
  std::size_t ticksSinceReport_{0};
  std::size_t reportedDiscardCount_{0};
  std::size_t reportedRequestDiscardCount_{0};
  std::size_t reportedDroppedCount_{0};
  std::size_t reportedWrongTokenCount_{0};
};

}  // namespace rtype::server
