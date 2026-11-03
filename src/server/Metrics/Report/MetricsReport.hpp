#pragma once

#include <vector>
#include "GamesCensus.hpp"
#include "SystemTime.hpp"
#include "TickDurationSummary.hpp"
#include "TrafficReport.hpp"

namespace rtype::server {

/** @brief What the server measured over TICKS_PER_REPORT ticks. */
struct MetricsReport {
  TickDurationSummary ticks;
  /** @brief The games as they stood at the end of those ticks. */
  GamesCensus games;
  /** @brief How long each system ran over those ticks, all games added up. */
  std::vector<engine::SystemTime> systemTimes;
  /** @brief What crossed the socket since the server started, by player and by
   * type of message. */
  network::TrafficReport traffic;
};

}  // namespace rtype::server
