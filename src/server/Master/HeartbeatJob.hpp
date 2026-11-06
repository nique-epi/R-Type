#pragma once

#include <stop_token>
#include <string>
#include <thread>
#include "HeartbeatJobParts.hpp"
#include "HeartbeatTimes.hpp"
#include "LatestHostedGames.hpp"
#include "LatestServerMeasures.hpp"
#include "Logger.hpp"
#include "MasterClient.hpp"
#include "ServerIdentity.hpp"

namespace rtype::server {

/**
 * @brief The job that keeps the master told of this server, on the
 * MasterLink thread, which it owns: it registers the server, then sends a
 * heartbeat each time the games change, at most one per noticeGap, and every
 * interval when nothing changes: it sleeps on the list of games until a newer
 * one or the end of the interval, so an idle server wakes once per interval.
 *
 * It never touches a game: it reads the list and the measures the supervised
 * games module publishes. A
 * master that refuses or does not answer is written in the journal once, not
 * at each try; the job registers again after times.retry, then goes on, so
 * the games play whatever the master does. MasterLink starts with the job and
 * stops, its call in progress ended within the timeouts of MasterClient, when
 * the job is destroyed.
 */
class HeartbeatJob {
 public:
  /** @throws network::InvalidMasterUrlException when the url of @p parts is
   * not `http://<host>[:<port>]`; MasterLink does not start then. */
  explicit HeartbeatJob(HeartbeatJobParts parts);

  HeartbeatJob(const HeartbeatJob&) = delete;
  HeartbeatJob& operator=(const HeartbeatJob&) = delete;
  HeartbeatJob(HeartbeatJob&&) = delete;
  HeartbeatJob& operator=(HeartbeatJob&&) = delete;
  ~HeartbeatJob() = default;

 private:
  void run(const std::stop_token& stop);
  /** @returns Whether the master took the registration. */
  [[nodiscard]] bool registerServer();
  /** @returns Whether the server stays registered after the heartbeat: false
   * when the master did not answer, or no longer knows the key or the
   * registration. */
  [[nodiscard]] bool sendHeartbeat();
  /** @brief Writes @p problem in the journal unless it was the last one. */
  void tell(const std::string& problem);

  network::MasterClient client_;
  ServerIdentity identity_;
  const LatestHostedGames& games_;
  const LatestServerMeasures* measures_;
  HeartbeatTimes times_;
  logging::Logger logger_;
  std::string lastProblem_;
  std::jthread masterLink_;
};

}  // namespace rtype::server
