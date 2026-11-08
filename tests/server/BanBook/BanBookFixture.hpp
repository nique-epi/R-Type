#pragma once

#include <gtest/gtest.h>
#include <filesystem>
#include "TemporaryFolder.hpp"

namespace rtype::server::testing {

/** @brief An empty folder, and the file of a server in it. */
class BanBookFixture : public ::testing::Test {
 protected:
  [[nodiscard]] std::filesystem::path file() const;

  /** @brief Drops the table of the bans of file() through a connection of its
   * own. */
  void dropTheBans() const;

 private:
  TemporaryFolder folder_;
};

}  // namespace rtype::server::testing
