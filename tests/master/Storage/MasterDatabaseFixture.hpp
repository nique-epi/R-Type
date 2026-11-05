#pragma once

#include <gtest/gtest.h>
#include <filesystem>
#include <string_view>
#include "MasterDatabase.hpp"
#include "TemporaryFolder.hpp"

/**
 * @brief A file of the master written at version 1, as a master of that
 * version left it, and the database that upgrades it when it opens it.
 */
class MasterDatabaseFixture : public ::testing::Test {
 protected:
  /** @brief Writes a file at version 1 holding "Paris 1", registered, its last
   * heartbeat @p lastHeartbeat received at VERSION_ONE_DATE. */
  void keepVersionOneServer(std::string_view lastHeartbeat) const;

  /** @returns The database, open on the file, so upgraded to the last
   * version. */
  [[nodiscard]] rtype::master::MasterDatabase& upgradedDatabase();

 private:
  [[nodiscard]] std::filesystem::path file() const;

  TemporaryFolder folder_;
  rtype::master::MasterDatabase database_;
};
