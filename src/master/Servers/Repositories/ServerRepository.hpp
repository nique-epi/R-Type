#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "CreationQuota.hpp"
#include "HeartbeatRecord.hpp"
#include "KnownServer.hpp"
#include "LastHeartbeat.hpp"
#include "MasterDatabase.hpp"
#include "NewServer.hpp"
#include "ServerRecord.hpp"

namespace rtype::master {

/**
 * @brief The table `servers`: the only code that writes its SQL. Each call runs
 * behind the lock of the MasterDatabase, so two requests never interleave.
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

  /** @brief Keeps the heartbeat of @p record as the last state of its server.
   */
  void saveLastHeartbeat(const HeartbeatRecord& record);

  /**
   * @returns The last heartbeat of the server @p serverId, nothing in it while
   * the server has sent none, or nothing at all when no server has that id.
   * @throws network::InvalidMasterMessageException when the text kept no longer
   * reads as a heartbeat.
   */
  [[nodiscard]] std::optional<LastHeartbeat> lastHeartbeatOf(
      std::int64_t serverId);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
