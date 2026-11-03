#include "LogLaunchOptionsFixtures.hpp"
#include <filesystem>
#include <span>
#include <string_view>
#include "LaunchOptionReading.hpp"
#include "LogLaunchOptions.hpp"

rtype::logging::LogLaunchOptions logOptionsOf(
    std::span<const std::string_view> arguments,
    const std::filesystem::path& defaultJournal) {
  rtype::logging::LogLaunchOptions options{defaultJournal};
  readEveryOption(options, arguments);
  return options;
}
