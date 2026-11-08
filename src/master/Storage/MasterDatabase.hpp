#pragma once

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include <mutex>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include "MasterException.hpp"

namespace rtype::master {

/**
 * @brief The connection to the database of the master, shared by the threads
 * of the HTTP server: one request uses it at a time, behind its lock.
 *
 * SQLite allows one writer at a time anyway. Most requests of the master hold
 * the lock for a few statements of one row each, so the threads queue for
 * microseconds; a read of the leaderboard holds it while it ranks every row of
 * one difficulty and one number of players (GameResultRepository). The
 * storage module opens it when it starts and closes it when it stops.
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

  /**
   * @returns What @p work returns, called with the connection inside one
   * immediate transaction while this holds the lock: the writes of @p work
   * commit together when it returns, and none of them when it throws, since
   * the transaction rolls back as the exception leaves. @p work calls the
   * functions of the repositories that take the connection, never one that
   * takes the lock again, which would wait for itself.
   * @throws DatabaseClosedException when the connection is not open.
   */
  template <typename Work>
  auto transact(Work&& work) {
    return run([&work](SQLite::Database& connection) {
      SQLite::Transaction transaction{connection,
                                      SQLite::TransactionBehavior::IMMEDIATE};
      if constexpr (std::is_void_v<
                        std::invoke_result_t<Work, SQLite::Database&>>) {
        std::forward<Work>(work)(connection);
        transaction.commit();
      } else {
        auto result = std::forward<Work>(work)(connection);
        transaction.commit();
        return result;
      }
    });
  }

 private:
  std::mutex mutex_;
  std::optional<SQLite::Database> connection_;
};

}  // namespace rtype::master
