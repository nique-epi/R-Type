#include "ServerStatusService.hpp"
#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstdint>
#include <vector>
#include "DatedStatus.hpp"
#include "MasterDatabase.hpp"
#include "ServerRepository.hpp"
#include "ServerStatus.hpp"
#include "ServerStatusRule.hpp"
#include "StatusChangeRecord.hpp"
#include "StatusChangeRepository.hpp"
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
          .change = network::DatedStatus{.since = now, .status = status}});
    }
  }
  return changes;
}

}  // namespace

ServerStatusService::ServerStatusService(MasterDatabase& database)
    : database_(database), servers_(database) {}

std::vector<StatusChangeRecord> ServerStatusService::refreshStatuses(
    std::chrono::sys_seconds now) {
  return database_.transact([now](SQLite::Database& connection) {
    std::vector<StatusChangeRecord> changes =
        changesAt(ServerRepository::watchedServers(connection), now);
    StatusChangeRepository::add(connection, changes, now);
    ServerRepository::saveStatuses(connection, changes);
    return changes;
  });
}

bool ServerStatusService::setMaintenance(std::int64_t serverId,
                                         bool maintenance) {
  return servers_.setMaintenance(serverId, maintenance);
}

}  // namespace rtype::master
