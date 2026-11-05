#pragma once

#include <SQLiteCpp/Database.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "CreationQuota.hpp"
#include "HeartbeatRecord.hpp"
#include "KnownServer.hpp"
#include "LastHeartbeat.hpp"
#include "ListedServer.hpp"
#include "MasterDatabase.hpp"
#include "NewServer.hpp"
#include "PageRequest.hpp"
#include "ServerRecord.hpp"
#include "StatusChangeRecord.hpp"
#include "WatchedServer.hpp"

namespace rtype::master {

/**
 * @brief The table `servers`: the only code that writes its SQL. Each call runs
 * behind the lock of the MasterDatabase, so two requests never interleave; a
 * static one runs on the connection its caller holds inside
 * MasterDatabase::transact(), so its writes join those of other tables.
 */
class ServerRepository {
 public:
  explicit ServerRepository(MasterDatabase& database);

  /**
   * @brief Adds @p server, unless its address created @p quota maximum servers
   * or more since @p quota since; the count and the insert are one
   * transaction, so requests at once cannot pass the quota together.
   * @returns The id of the server added, or nothing over the quota.
   */
  std::optional<std::int64_t> insertWithin(const NewServer& server,
                                           const CreationQuota& quota);

  /** @returns The server whose key has @p fingerprint, or nothing. */
  [[nodiscard]] std::optional<KnownServer> serverOfKey(
      const std::string& fingerprint);

  /**
   * @brief Keeps the registration of @p record as the representation of its
   * server, forgetting its last heartbeat: a server registers when it starts,
   * so it hosts no game yet.
   */
  void saveRegistration(const ServerRecord& record);

  /** @brief Keeps the heartbeat of @p record as the last state of its server,
   * with the counts and the measures the list and the statuses read of it, on
   * @p connection. */
  static void saveLastHeartbeat(SQLite::Database& connection,
                                const HeartbeatRecord& record);

  /**
   * @returns The last heartbeat of the server @p serverId, nothing in it while
   * the server has sent none, or nothing at all when no server has that id.
   * @throws network::InvalidMasterMessageException when the text kept no longer
   * reads as a heartbeat.
   */
  [[nodiscard]] std::optional<LastHeartbeat> lastHeartbeatOf(
      std::int64_t serverId);

  /**
   * @returns Every registered server, by id, with what its status is computed
   * from and the status it holds, read on @p connection.
   */
  [[nodiscard]] static std::vector<WatchedServer> watchedServers(
      SQLite::Database& connection);

  /** @brief Keeps the status of each of @p changes as the status of its
   * server, on @p connection. */
  static void saveStatuses(SQLite::Database& connection,
                           const std::vector<StatusChangeRecord>& changes);

  /**
   * @brief Puts the server @p serverId in maintenance, or takes it out of it.
   * @returns Whether a server has that id.
   */
  bool setMaintenance(std::int64_t serverId, bool maintenance);

  /**
   * @returns The registered servers of @p page, by id, each with the counts of
   * its last heartbeat: 0 and 0 before the first one.
   */
  [[nodiscard]] std::vector<network::ListedServer> registeredServers(
      const PageRequest& page);

  /** @returns Whether a server, registered or not, has the id @p serverId. */
  [[nodiscard]] bool hasServer(std::int64_t serverId);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
