#include <gtest/gtest.h>
#include <array>
#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>
#include "LaunchArguments.hpp"
#include "LaunchArgumentsTestConstants.hpp"

using rtype::engine::pathOption;
using rtype::engine::readLaunchArguments;

/**
 * Given an option of a path, and no argument
 * When the arguments are read
 * Then the path holds nothing
 */
TEST(PathOption, HoldsNothingWhenItIsNotGiven) {
  std::optional<std::filesystem::path> path;
  const std::array options{pathOption(VALUE_OPTION, path)};

  readLaunchArguments(std::vector<std::string_view>{}, options);

  EXPECT_FALSE(path.has_value());
}

/**
 * Given an option of a path, given with a value
 * When the arguments are read
 * Then the path holds that value
 */
TEST(PathOption, HoldsTheValueItIsGiven) {
  std::optional<std::filesystem::path> path;
  const std::array options{pathOption(VALUE_OPTION, path)};

  readLaunchArguments(std::vector<std::string_view>{VALUE_OPTION, FIRST_VALUE},
                      options);

  EXPECT_EQ(path.value_or(std::filesystem::path{}),
            std::filesystem::path{FIRST_VALUE});
}

/**
 * Given an option of a path, given twice
 * When the arguments are read
 * Then the path holds the last value
 */
TEST(PathOption, KeepsTheLastValue) {
  std::optional<std::filesystem::path> path;
  const std::array options{pathOption(VALUE_OPTION, path)};

  readLaunchArguments(std::vector<std::string_view>{VALUE_OPTION, FIRST_VALUE,
                                                    VALUE_OPTION, SECOND_VALUE},
                      options);

  EXPECT_EQ(path.value_or(std::filesystem::path{}),
            std::filesystem::path{SECOND_VALUE});
}
