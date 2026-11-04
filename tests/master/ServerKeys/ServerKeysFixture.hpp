#pragma once

#include <gtest/gtest.h>
#include <string_view>
#include "MasterDatabase.hpp"
#include "ServerKeys.hpp"
#include "TemporaryFolder.hpp"

/** @brief The keys of the master on a database of their own, in a folder. */
class ServerKeysFixture : public ::testing::Test {
 protected:
  ServerKeysFixture();

  [[nodiscard]] rtype::master::ServerKeys& keys();

  /** @brief Closes the database and opens it again, as a restart does. */
  void restart();

  /**
   * @returns Whether @p text is written anywhere in the files of the
   * database, its log included.
   */
  [[nodiscard]] bool databaseFilesHold(std::string_view text) const;

 private:
  TemporaryFolder folder_;
  rtype::master::MasterDatabase database_;
  rtype::master::ServerKeys keys_{database_};
};
