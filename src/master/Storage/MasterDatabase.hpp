#pragma once

#include <SQLiteCpp/Database.h>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include "MasterException.hpp"

namespace rtype::master {

/**
 * @brief The connection to the database of the master, shared by the threads
 * of the HTTP server: one request uses it at a time, behind its lock.
 *
 * SQLite allows one writer at a time anyway, and a request of the master holds
 * the lock for a few statements of one row each, so the threads queue for
 * microseconds. The storage module opens it when it starts and closes it when
 * it stops.
 */
class MasterDatabase {
 public:
  /**
   * @brief Opens @p file, a path in UTF-8, and applies the MASTER_MIGRATIONS
   * it lacks (see storage::openDatabaseFile()).
   * @throws storage::StorageException when it cannot be opened or migrated.
   */
  void open(const std::string& file);

  /** @brief Closes the connection, once the request in progress finished. */
  void close();

  /**
   * @returns What @p work returns, called with the connection while this
   * holds the lock.
   * @throws DatabaseClosedException when the connection is not open.
   */
  template <typename Work>
  auto run(Work&& work) {
    const std::scoped_lock lock{mutex_};
    if (!connection_.has_value()) {
      throw DatabaseClosedException();
    }
    return std::forward<Work>(work)(*connection_);
  }

 private:
  std::mutex mutex_;
  std::optional<SQLite::Database> connection_;
};

}  // namespace rtype::master
