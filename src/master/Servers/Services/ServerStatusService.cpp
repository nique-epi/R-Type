#include "ServerStatusService.hpp"
#include <chrono>
#include <cstdint>
#include <vector>
#include "MasterDatabase.hpp"
#include "ServerStatus.hpp"
#include "ServerStatusChange.hpp"
#include "ServerStatusRule.hpp"
#include "StatusChangeRecord.hpp"
#include "WatchedServer.hpp"

namespace rtype::master {

namespace {

/** @returns A change for each server of @p watched whose status at @p now
 * differs from the one it holds, by server id. */
std::vector<StatusChangeRecord> changesAt(
    const std::vector<WatchedServer>& watched, std::chrono::sys_seconds now) {
  std::vector<StatusChangeRecord> changes;
  for (const WatchedServer& server : watched) {
    const network::ServerStatus status = statusOf(server.evidence, now);
    if (status != server.status) {
      changes.push_back(StatusChangeRecord{
          .serverId = server.serverId,
          .change =
              network::ServerStatusChange{.changedAt = now, .status = status}});
    }
  }
  return changes;
}

}  // namespace

ServerStatusService::ServerStatusService(MasterDatabase& database)
    : servers_(database), changes_(database) {}

std::vector<StatusChangeRecord> ServerStatusService::refreshStatuses(
    std::chrono::sys_seconds now) {
  std::vector<StatusChangeRecord> changes =
      changesAt(servers_.watchedServers(), now);
  changes_.add(changes);
  servers_.saveStatuses(changes);
  return changes;
}

bool ServerStatusService::setMaintenance(std::int64_t serverId,
                                         bool maintenance) {
  return servers_.setMaintenance(serverId, maintenance);
}

}  // namespace rtype::master
