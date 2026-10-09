#include <gtest/gtest.h>
#include <filesystem>
#include <optional>
#include "ExecutablePath.hpp"

using rtype::client::executablePath;

namespace {

constexpr const char* TEST_EXECUTABLE_NAME = "client_tests";

}  // namespace

/**
 * Given the running test executable
 * When the system is asked for the executable path
 * Then it names this executable, which exists on disk
 */
TEST(ExecutablePath, NamesTheRunningExecutable) {
  const std::optional<std::filesystem::path> executable = executablePath();
  const std::filesystem::path reported = executable.value_or("");

  ASSERT_TRUE(executable.has_value());
  EXPECT_EQ(reported.stem(), TEST_EXECUTABLE_NAME);
  EXPECT_TRUE(std::filesystem::is_regular_file(reported));
}
