#pragma once

#include <functional>
#include "HeartbeatTimes.hpp"
#include "LatestHostedGames.hpp"
#include "LatestServerMeasures.hpp"
#include "MasterAccess.hpp"
#include "ServerIdentity.hpp"

namespace rtype::server {

/** @brief What a HeartbeatJob is built from. */
struct HeartbeatJobParts {
  /** @brief Where the master answers, and the key of the server. */
  network::MasterAccess access;
  ServerIdentity identity;
  /** @brief The games, which the job reads and waits on; they must outlive
   * it. */
  std::reference_wrapper<const LatestHostedGames> games;
  /** @brief The measures the heartbeat carries; must outlive the job. Empty,
   * or nothing published yet: the heartbeat carries no measure. */
  const LatestServerMeasures* measures{nullptr};
  HeartbeatTimes times{};
};

}  // namespace rtype::server
