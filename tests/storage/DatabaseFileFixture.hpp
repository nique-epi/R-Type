#pragma once

#include <SQLiteCpp/Database.h>
#include <gtest/gtest.h>
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

  /** @returns The whole number @p query answers on @p database. */
  [[nodiscard]] static int numberFrom(SQLite::Database& database,
                                      std::string_view query);

 private:
  TemporaryFolder folder_;
};
