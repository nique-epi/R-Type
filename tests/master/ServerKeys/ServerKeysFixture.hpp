#pragma once

#include <gtest/gtest.h>
#include <string_view>
#include "MasterDatabaseFile.hpp"
#include "ServerKeys.hpp"

/** @brief The keys of the master on a database of their own, in a folder. */
class ServerKeysFixture : public ::testing::Test {
 protected:
  [[nodiscard]] rtype::master::ServerKeys& keys();

  /** @brief Closes the database and opens it again, as a restart does. */
  void restart();

  /**
   * @returns Whether @p text is written anywhere in the files of the
   * database, its log included.
   */
  [[nodiscard]] bool databaseFilesHold(std::string_view text) const;

 private:
  MasterDatabaseFile databaseFile_;
  rtype::master::ServerKeys keys_{databaseFile_.database()};
};
