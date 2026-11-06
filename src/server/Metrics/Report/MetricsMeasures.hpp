#pragma once

#include "MetricsReport.hpp"
#include "ServerMeasures.hpp"

namespace rtype::server {

/**
 * @returns The measures of @p report the master reads: the median and 95th
 * percentile of the ticks, and the mean round trip of the players; a measure
 * the report has not taken stays empty.
 */
[[nodiscard]] network::ServerMeasures measuresOf(const MetricsReport& report);

}  // namespace rtype::server
