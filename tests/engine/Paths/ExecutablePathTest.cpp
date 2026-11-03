#include <gtest/gtest.h>
#include <filesystem>
#include <optional>
#include "ExecutablePath.hpp"
#include "ExecutablePathTestConstants.hpp"

using rtype::engine::executableFolder;
using rtype::engine::executablePath;

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

/**
 * Given the running test executable, which the build wrote in its tests folder
 * When the folder of the executable is asked for
 * Then it is that tests folder
 */
TEST(ExecutablePath, FolderIsWhereTheBuildWroteTheExecutable) {
  EXPECT_TRUE(std::filesystem::equivalent(
      executableFolder(), std::filesystem::path{RTYPE_TEST_EXECUTABLE_FOLDER}));
}
