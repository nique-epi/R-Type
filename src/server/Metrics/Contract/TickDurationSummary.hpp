#pragma once

#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief What the durations of a series of ticks say. A percentile is the
 * duration at the nearest rank, so it is always the duration of a real tick:
 * the smallest one that at least that percent of the ticks do not exceed. All
 * zero when the series holds no tick.
 */
struct TickDurationSummary {
  std::size_t count{0};
  engine::Duration average{};
  engine::Duration median{};
  engine::Duration ninetyFifthPercentile{};
  engine::Duration worst{};
};

}  // namespace rtype::server
