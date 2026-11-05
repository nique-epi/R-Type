#pragma once

#include <cstdint>
#include <vector>
#include "MasterDatabase.hpp"
#include "PageRequest.hpp"
#include "ServerStatusChange.hpp"
#include "StatusChangeRecord.hpp"

namespace rtype::master {

/**
 * @brief The table `status_changes`: the only code that writes its SQL. Each
 * call runs behind the lock of the MasterDatabase.
 */
class StatusChangeRepository {
 public:
  explicit StatusChangeRepository(MasterDatabase& database);

  /**
   * @brief Adds each of @p changes, in place of a change of its server in the
   * same second, then erases every change older than STATUS_CHANGE_RETENTION
   * before the first of them, of every server; one transaction, none for no
   * change.
   */
  void add(const std::vector<StatusChangeRecord>& changes);

  /** @returns The changes of the server @p serverId in @p page, newest
   * first. */
  [[nodiscard]] std::vector<network::ServerStatusChange> changesOf(
      std::int64_t serverId, const PageRequest& page);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
