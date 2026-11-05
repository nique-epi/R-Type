#pragma once

#include <chrono>
#include "ServerMeasures.hpp"

namespace rtype::master {

/** @brief What the status of a registered server is computed from. */
struct StatusEvidence {
  /**
   * @brief The second the master last heard of it: its last heartbeat, or its
   * registration while it sent none since.
   */
  std::chrono::sys_seconds lastHeardAt;
  /** @brief The measures of its last heartbeat; none before the first one, or
   * when the heartbeat kept no longer reads. */
  network::ServerMeasures measures;
  /** @brief Whether an admin took it out of service. */
  bool maintenance{false};
};

}  // namespace rtype::master
