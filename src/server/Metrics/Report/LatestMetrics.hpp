#pragma once

#include <mutex>
#include <optional>
#include "MetricsReport.hpp"

namespace rtype::server {

/**
 * @brief The newest MetricsReport, the service the other parts of the server
 * read the metrics from.
 *
 * Reading changes nothing, so readers at different rates all see the same
 * report. The simulation thread publishes and any thread may read: the lock is
 * held for a move or a copy only, so a slow reader never slows a tick.
 */
class LatestMetrics {
 public:
  /** @brief Replaces the report held by @p report. */
  void publish(MetricsReport report);

  /** @returns A copy of the newest report, or nothing before the first one. */
  [[nodiscard]] std::optional<MetricsReport> report() const;

 private:
  mutable std::mutex mutex_;
  std::optional<MetricsReport> report_;
};

}  // namespace rtype::server
