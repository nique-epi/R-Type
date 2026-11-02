#pragma once

#include <vector>
#include "GamesCensus.hpp"
#include "SystemTime.hpp"
#include "TickDurationSummary.hpp"

namespace rtype::server {

/** @brief What the server measured over TICKS_PER_REPORT ticks. */
struct MetricsReport {
  TickDurationSummary ticks;
  /** @brief The games as they stood at the end of those ticks. */
  GamesCensus games;
  /** @brief How long each system ran over those ticks, all games added up. */
  std::vector<engine::SystemTime> systemTimes;
};

}  // namespace rtype::server
