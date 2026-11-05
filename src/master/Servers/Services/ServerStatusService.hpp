#pragma once

#include <chrono>
#include <cstdint>
#include <vector>
#include "MasterDatabase.hpp"
#include "ServerRepository.hpp"
#include "StatusChangeRecord.hpp"

namespace rtype::master {

/** @brief The rule of the statuses of the game servers: their refresh, and the
 * maintenance an admin sets. */
class ServerStatusService {
 public:
  explicit ServerStatusService(MasterDatabase& database);

  /**
   * @brief Computes the status of every registered server at @p now
   * (statusOf()) and keeps each one that differs from the status the server
   * holds, as a change of its history and as its status, then erases the
   * changes older than STATUS_CHANGE_RETENTION. The read and the writes are
   * one transaction under the lock of the database: a heartbeat waits for the
   * refresh to end, so no status is computed from a server heard again
   * meanwhile, and a refresh that fails keeps no change and no status.
   * @returns The changes kept, by server id.
   */
  std::vector<StatusChangeRecord> refreshStatuses(std::chrono::sys_seconds now);

  /**
   * @brief Puts the server @p serverId in maintenance, or takes it out of it;
   * its status follows at the next refresh.
   * @returns Whether a server has that id.
   */
  bool setMaintenance(std::int64_t serverId, bool maintenance);

 private:
  MasterDatabase& database_;
  ServerRepository servers_;
};

}  // namespace rtype::master
