#pragma once

#include <cstdint>
#include <optional>
#include "HeartbeatRecord.hpp"
#include "MasterDatabase.hpp"
#include "RegisteredServer.hpp"
#include "ServerLimits.hpp"
#include "ServerRecord.hpp"

namespace rtype::master {

/**
 * @brief The tables `servers` and `heartbeat_samples`: the only code that
 * writes their SQL. Each call runs in one transaction behind the lock of the
 * MasterDatabase, so two requests of one server never interleave.
 */
class ServerRepository {
 public:
  explicit ServerRepository(MasterDatabase& database);

  /**
   * @brief Adds the server of @p record, or updates the one its key already
   * registered, forgetting its last heartbeat: a server registers when it
   * starts, so it hosts no game yet.
   */
  RegisteredServer save(const ServerRecord& record);

  /** @returns The server the key @p keyId registered, or nothing. */
  [[nodiscard]] std::optional<ServerLimits> limitsOfKey(std::int64_t keyId);

  /**
   * @brief Keeps the heartbeat of @p record as the last state of its server,
   * adds a sample of it unless one of that second exists, and erases every
   * sample older than SAMPLE_RETENTION, of every server.
   */
  void recordHeartbeat(const HeartbeatRecord& record);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
