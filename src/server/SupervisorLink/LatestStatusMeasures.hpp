#pragma once

#include <cstdint>
#include "GameStatusMeasures.hpp"
#include "LatestMetrics.hpp"

namespace rtype::server {

/**
 * @brief The measures a game process tells its supervisor, taken from the
 * newest MetricsReport: the median and the 95th percentile of its ticks and
 * the mean round trip of its players, in whole microseconds. They are worked
 * out again only when the metrics module published a newer report.
 */
class LatestStatusMeasures {
 public:
  /** @param metrics The reports of the metrics module; nullptr without it,
   * and the measures stay empty. It must outlive this object. */
  explicit LatestStatusMeasures(const LatestMetrics* metrics);

  /** @returns The measures of the newest report; none before the first one. */
  [[nodiscard]] const network::GameStatusMeasures& current();

 private:
  const LatestMetrics* metrics_;
  std::uint64_t measuredRevision_{0};
  network::GameStatusMeasures measures_{};
};

}  // namespace rtype::server
