#include "LogLaunchOptions.hpp"
#include <array>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "LoggingException.hpp"

namespace rtype::logging {

namespace {

struct LaunchSettings {
  std::optional<LogLevel> level;
  LogOutput output;
};

struct LaunchOption {
  std::string_view name;
  bool takesValue;
  void (*record)(LaunchSettings& settings, std::string_view value);
};

void recordLevel(LaunchSettings& settings, std::string_view levelName) {
  settings.level = Logger::parseLevel(levelName);
  if (!settings.level.has_value()) {
    throw UnknownLogLevelException(std::string(levelName));
  }
}

void recordFilePath(LaunchSettings& settings, std::string_view filePath) {
  settings.output.filePath = std::filesystem::path(filePath);
}

void recordNoFile(LaunchSettings& settings,
                  [[maybe_unused]] std::string_view value) {
  settings.output.filePath.reset();
}

void recordStderr(LaunchSettings& settings,
                  [[maybe_unused]] std::string_view value) {
  settings.output.toStderr = true;
}

constexpr std::array launchOptions{
    LaunchOption{
        .name = LEVEL_OPTION, .takesValue = true, .record = recordLevel},
    LaunchOption{
        .name = FILE_OPTION, .takesValue = true, .record = recordFilePath},
    LaunchOption{
        .name = NO_FILE_OPTION, .takesValue = false, .record = recordNoFile},
    LaunchOption{
        .name = STDERR_OPTION, .takesValue = false, .record = recordStderr},
};

std::optional<LaunchOption> findLaunchOption(std::string_view argument) {
  for (const LaunchOption& option : launchOptions) {
    if (option.name == argument) {
      return option;
    }
  }
  return std::nullopt;
}

bool isLoggingOption(std::string_view argument) {
  return argument.starts_with(LOG_OPTION_PREFIX) ||
         argument.starts_with(NO_LOG_OPTION_PREFIX);
}

std::string_view valueAfter(std::span<const std::string_view> arguments,
                            std::size_t& index) {
  const std::size_t valueIndex = index + 1;
  if (valueIndex >= arguments.size()) {
    throw MissingLogOptionValueException(std::string(arguments[index]));
  }
  index = valueIndex;
  return arguments[valueIndex];
}

}  // namespace

void LogLaunchOptions::apply(std::span<const std::string_view> arguments,
                             std::string_view defaultFileName) {
  LaunchSettings settings{
      .level = std::nullopt,
      .output = LogOutput{.toStderr = false,
                          .filePath = std::filesystem::path(defaultFileName)}};
  for (std::size_t index = 0; index < arguments.size(); ++index) {
    const std::string_view argument = arguments[index];
    const std::optional<LaunchOption> option = findLaunchOption(argument);
    if (option.has_value()) {
      const std::string_view value = option->takesValue
                                         ? valueAfter(arguments, index)
                                         : std::string_view{};
      option->record(settings, value);
    } else if (isLoggingOption(argument)) {
      throw InvalidLogOptionException(std::string(argument));
    }
  }
  Logger::setOutput(settings.output);
  if (settings.level.has_value()) {
    Logger::setLevel(*settings.level);
  }
}

void LogLaunchOptions::applyFromProcess(int argumentCount,
                                        const char* const* arguments,
                                        std::string_view defaultFileName) {
  std::vector<std::string_view> options;
  for (int index = 1; index < argumentCount; ++index) {
    options.emplace_back(arguments[index]);
  }
  apply(options, defaultFileName);
}

}  // namespace rtype::logging
