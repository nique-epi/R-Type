#include "LogLaunchOptions.hpp"
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
  std::optional<LogLevel> level;
  LogOutput output{.toStderr = false,
                   .filePath = std::filesystem::path(defaultFileName)};
  for (std::size_t index = 0; index < arguments.size(); ++index) {
    const std::string_view argument = arguments[index];
    if (argument == LEVEL_OPTION) {
      const std::string_view name = valueAfter(arguments, index);
      level = Logger::parseLevel(name);
      if (!level.has_value()) {
        throw UnknownLogLevelException(std::string(name));
      }
    } else if (argument == FILE_OPTION) {
      output.filePath = std::filesystem::path(valueAfter(arguments, index));
    } else if (argument == NO_FILE_OPTION) {
      output.filePath.reset();
    } else if (argument == STDERR_OPTION) {
      output.toStderr = true;
    } else if (argument.starts_with(LOG_OPTION_PREFIX) ||
               argument.starts_with(NO_LOG_OPTION_PREFIX)) {
      throw InvalidLogOptionException(std::string(argument));
    }
  }
  Logger::setOutput(output);
  if (level.has_value()) {
    Logger::setLevel(*level);
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
