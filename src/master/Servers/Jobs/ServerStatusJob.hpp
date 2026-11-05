#pragma once

#include <chrono>
#include <stop_token>
#include <thread>
#include <vector>
#include "Logger.hpp"
#include "MasterDatabase.hpp"
#include "ServerStatusService.hpp"
#include "StatusChangeRecord.hpp"

namespace rtype::master {

/**
 * @brief The job that keeps the status of every registered server, on the
 * StatusWatch thread, which it owns: it refreshes the statuses as soon as it
 * is built, then every interval, and writes each change in the journal.
 *
 * A refresh that fails, the database locked or full, is written in the
 * journal and tried again an interval later: an exception that left the
 * thread would end the master. StatusWatch stops when the job is destroyed,
 * once the refresh in progress is done; a sleep between two refreshes ends at
 * once.
 */
class ServerStatusJob {
 public:
  /** @param interval The time between two refreshes: STATUS_REFRESH_INTERVAL,
   * shorter in the tests. */
  ServerStatusJob(MasterDatabase& database, std::chrono::milliseconds interval);

  ServerStatusJob(const ServerStatusJob&) = delete;
  ServerStatusJob& operator=(const ServerStatusJob&) = delete;
  ServerStatusJob(ServerStatusJob&&) = delete;
  ServerStatusJob& operator=(ServerStatusJob&&) = delete;
  ~ServerStatusJob() = default;

 private:
  void run(const std::stop_token& stop);
  /** @returns The changes of one refresh at the second of the clock, none
   * when it failed, which is written in the journal. */
  [[nodiscard]] std::vector<StatusChangeRecord> refresh();
  /** @brief Writes each change of @p changes in the journal. */
  void tell(const std::vector<StatusChangeRecord>& changes) const;

  ServerStatusService service_;
  std::chrono::milliseconds interval_;
  logging::Logger logger_;
  std::jthread statusWatch_;
};

}  // namespace rtype::master
