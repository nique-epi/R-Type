#pragma once

#include <chrono>
#include <cstdint>
#include <vector>
#include "MasterDatabase.hpp"
#include "ServerRepository.hpp"
#include "StatusChangeRecord.hpp"
#include "StatusChangeRepository.hpp"

namespace rtype::master {

/** @brief The rule of the statuses of the game servers: their refresh, and the
 * maintenance an admin sets. */
class ServerStatusService {
 public:
  explicit ServerStatusService(MasterDatabase& database);

  /**
   * @brief Computes the status of every registered server at @p now
   * (statusOf()) and keeps each one that differs from the status the server
   * holds: first as a change of its history, then as its status. A stop
   * between the two leaves the change kept and the old status, which the next
   * refresh writes again; never a status without its change.
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
  ServerRepository servers_;
  StatusChangeRepository changes_;
};

}  // namespace rtype::master
