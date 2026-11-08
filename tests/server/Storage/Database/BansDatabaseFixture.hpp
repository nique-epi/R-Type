#pragma once

#include <gtest/gtest.h>
#include <filesystem>
#include "TemporaryFolder.hpp"

namespace rtype::server::testing {

/** @brief An empty folder and the database of a server in it. */
class BansDatabaseFixture : public ::testing::Test {
 protected:
  [[nodiscard]] std::filesystem::path file() const;

 private:
  TemporaryFolder folder_;
};

}  // namespace rtype::server::testing
