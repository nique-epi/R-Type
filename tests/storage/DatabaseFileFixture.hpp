#pragma once

#include <SQLiteCpp/Database.h>
#include <gtest/gtest.h>
#include <span>
#include <string>
#include <string_view>
#include "TemporaryFolder.hpp"

/** @brief An empty folder and a file name in it, for the tests of the opening
 * of a database. */
class DatabaseFileFixture : public ::testing::Test {
 protected:
  [[nodiscard]] std::string file() const;
  /** @returns A file in a folder that does not exist. */
  [[nodiscard]] std::string fileInAMissingFolder() const;

  /**
   * @brief Opens file() with @p migrations while another connection holds a
   * write transaction on it, in which it ran @p write, and commits it
   * WRITE_HELD_FOR later. The other connection waits for a lock like every
   * connection of the project: its commit needs the opening to let go of its
   * read lock, which can take a moment, and without that wait the commit fails
   * with "database is locked" while the opening retries until its timeout.
   * @returns The connection openDatabaseFile() gave; what it threw, rethrown.
   */
  [[nodiscard]] SQLite::Database openWhileAnotherConnectionWrites(
      std::span<const std::string_view> migrations,
      std::string_view write) const;

  /** @returns The whole number @p query answers on @p database. */
  [[nodiscard]] static int numberFrom(SQLite::Database& database,
                                      std::string_view query);

 private:
  TemporaryFolder folder_;
};
