#pragma once

#include <chrono>
#include <optional>
#include "ServerMeasures.hpp"

namespace rtype::master {

/** @brief What the status of a registered server is computed from. */
struct StatusEvidence {
  /** @brief The second it registered last. */
  std::chrono::sys_seconds registeredAt;
  /** @brief The second of its last heartbeat; none while it sent none since
   * it registered. */
  std::optional<std::chrono::sys_seconds> lastHeartbeatAt = std::nullopt;
  /** @brief The tick p95 and the loss of its last heartbeat, the other
   * measures left empty; none before the first heartbeat. */
  network::ServerMeasures measures;
  /** @brief Whether an admin took it out of service. */
  bool maintenance{false};
  /** @brief The second of its last valid answer to a probe; none since it
   * registered. */
  std::optional<std::chrono::sys_seconds> lastProbeAnswerAt = std::nullopt;
};

}  // namespace rtype::master
