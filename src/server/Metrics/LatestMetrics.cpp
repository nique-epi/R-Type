#include "LatestMetrics.hpp"
#include <mutex>
#include <optional>
#include <utility>
#include "MetricsReport.hpp"

namespace rtype::server {

void LatestMetrics::publish(MetricsReport report) {
  const std::scoped_lock lock{mutex_};
  report_ = std::move(report);
}

std::optional<MetricsReport> LatestMetrics::report() const {
  const std::scoped_lock lock{mutex_};
  return report_;
}

}  // namespace rtype::server
