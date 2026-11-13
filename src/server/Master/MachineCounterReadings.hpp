#pragma once

#include <optional>
#include "MachineCounters.hpp"

namespace rtype::server {

/** @brief The counters of the machine read for two heartbeats in a row; each
 * is empty when the system gave none, and the earlier one before the first
 * heartbeat. */
struct MachineCounterReadings {
  std::optional<MachineCounters> earlier;
  std::optional<MachineCounters> latest;
};

}  // namespace rtype::server
