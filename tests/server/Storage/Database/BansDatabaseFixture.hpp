#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <filesystem>
#include <optional>
#include <string_view>
#include "Ban.hpp"
#include "BanKind.hpp"
#include "TemporaryFolder.hpp"

namespace rtype::server::testing {

/** @returns A ban of @p text as @p kind, from BAN_MOMENT until @p end. */
[[nodiscard]] Ban banOf(BanKind kind, std::string_view text,
                        std::optional<std::chrono::sys_seconds> end);

/** @brief An empty folder and the database of a server in it. */
class BansDatabaseFixture : public ::testing::Test {
 protected:
  [[nodiscard]] std::filesystem::path file() const;

 private:
  TemporaryFolder folder_;
};

}  // namespace rtype::server::testing
