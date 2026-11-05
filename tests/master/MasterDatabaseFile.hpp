#pragma once

#include <filesystem>
#include "MasterDatabase.hpp"
#include "TemporaryFolder.hpp"

/**
 * @brief A database of the master in a folder of its own, open from its
 * construction, both removed with it.
 */
class MasterDatabaseFile {
 public:
  MasterDatabaseFile();

  [[nodiscard]] rtype::master::MasterDatabase& database();

  /** @returns The file of the database, TEST_DATABASE_FILE in the folder. */
  [[nodiscard]] std::filesystem::path file() const;

  /** @brief Closes the database and opens it again, as a restart does. */
  void restart();

  /** @brief Closes the database, so a master of the test opens the file. */
  void close();

 private:
  TemporaryFolder folder_;
  rtype::master::MasterDatabase database_;
};
