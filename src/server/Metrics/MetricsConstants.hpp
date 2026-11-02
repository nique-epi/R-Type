#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::server {

/** @brief Module name the metrics of the server write their log lines under. */
constexpr std::string_view METRICS_LOGGER_NAME = "Metrics";

/** @brief The percentiles a report gives of the durations of the ticks, and the
 * whole they are counted out of. */
constexpr std::size_t MEDIAN_PERCENT = 50;
constexpr std::size_t NINETY_FIFTH_PERCENT = 95;
constexpr std::size_t WHOLE_PERCENT = 100;

}  // namespace rtype::server
