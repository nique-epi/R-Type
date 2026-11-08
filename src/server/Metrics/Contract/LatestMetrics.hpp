#pragma once

#include "LatestValue.hpp"
#include "MetricsReport.hpp"

namespace rtype::server {

/**
 * @brief The newest MetricsReport, the service the other parts of the server
 * read the metrics from: the simulation thread publishes, any thread reads.
 */
using LatestMetrics = concurrency::LatestValue<MetricsReport>;

}  // namespace rtype::server
