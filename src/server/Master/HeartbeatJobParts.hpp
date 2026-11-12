#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <vector>
#include "HeartbeatTimes.hpp"
#include "LatestHostedGames.hpp"
#include "LatestServerMeasures.hpp"
#include "LatestValue.hpp"
#include "MachineCounters.hpp"
#include "MasterAccess.hpp"
#include "ServerIdentity.hpp"

namespace rtype::server {

/**
 * @brief What a HeartbeatJob is built from.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct HeartbeatJobParts {
  /** @brief Where the master answers, and the key of the server. */
  network::MasterAccess access;
  ServerIdentity identity;
  /** @brief The games, which the job reads and waits on; they must outlive
   * it. */
  std::reference_wrapper<const LatestHostedGames> games;
  /** @brief The difficulties the games may be played at, which every
   * heartbeat carries; they do not change while the job runs. */
  std::vector<std::string> difficulties;
  /** @brief The tokens of the probes of the master, which the job reads;
   * they must outlive it. */
  std::reference_wrapper<const concurrency::LatestValue<std::uint64_t>>
      probeTokens;
  /** @brief The measures the heartbeat carries; must outlive the job. Empty,
   * or nothing published yet: the heartbeat carries no measure. */
  const LatestServerMeasures* measures{nullptr};
  HeartbeatTimes times{};
  /** @brief The file of the results of the games, whose vanilla games the job
   * sends after each heartbeat (see ResultsCourier). */
  std::filesystem::path resultsFile;
  /** @brief Reads the counters of the machine, once per heartbeat, on the
   * MasterLink thread: countersOfThisMachine() in the server, counters of
   * its own in a test. */
  std::function<std::optional<MachineCounters>()> machineCounters;
};

}  // namespace rtype::server
