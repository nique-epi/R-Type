#include "LogLaunchOptions.hpp"
#include <array>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
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

LogLaunchOptions LogLaunchOptions::read(
    std::span<const std::string_view> arguments,
    std::string_view defaultFileName) {
  LaunchSettings settings{
      .level = std::nullopt,
      .output = LogOutput{.toStderr = false,
                          .filePath = std::filesystem::path(defaultFileName)}};
  std::vector<std::string_view> otherArguments;
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
    } else {
      otherArguments.push_back(argument);
    }
  }
  return LogLaunchOptions{settings.level, settings.output,
                          std::move(otherArguments)};
}

LogLaunchOptions LogLaunchOptions::readFromProcess(
    int argumentCount, const char* const* arguments,
    std::string_view defaultFileName) {
  std::vector<std::string_view> processArguments;
  for (int index = 1; index < argumentCount; ++index) {
    processArguments.emplace_back(arguments[index]);
  }
  return read(processArguments, defaultFileName);
}

void LogLaunchOptions::apply() const {
  Logger::setOutput(output_);
  if (level_.has_value()) {
    Logger::setLevel(*level_);
  }
}

std::span<const std::string_view> LogLaunchOptions::otherArguments() const {
  return otherArguments_;
}

LogLaunchOptions::LogLaunchOptions(std::optional<LogLevel> level,
                                   LogOutput output,
                                   std::vector<std::string_view> otherArguments)
    : level_(level),
      output_(std::move(output)),
      otherArguments_(std::move(otherArguments)) {}

}  // namespace rtype::logging
