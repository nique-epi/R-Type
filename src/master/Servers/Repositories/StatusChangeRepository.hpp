#pragma once

#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstdint>
#include <vector>
#include "DatedStatus.hpp"
#include "MasterDatabase.hpp"
#include "PageRequest.hpp"
#include "StatusChangeRecord.hpp"

namespace rtype::master {

/**
 * @brief The table `status_changes`: the only code that writes its SQL. Each
 * call runs behind the lock of the MasterDatabase; a static one runs on the
 * connection its caller holds inside MasterDatabase::transact().
 */
class StatusChangeRepository {
 public:
  explicit StatusChangeRepository(MasterDatabase& database);

  /**
   * @brief Adds each of @p changes, in place of a change of its server in the
   * same second, then erases every change older than STATUS_CHANGE_RETENTION
   * before @p now, of every server, even when @p changes is empty; on
   * @p connection.
   */
  static void add(SQLite::Database& connection,
                  const std::vector<StatusChangeRecord>& changes,
                  std::chrono::sys_seconds now);

  /** @returns The changes of the server @p serverId in @p page, newest
   * first. */
  [[nodiscard]] std::vector<network::DatedStatus> changesOf(
      std::int64_t serverId, const PageRequest& page);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
