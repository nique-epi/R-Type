#include "LogLaunchOptions.hpp"
#include <array>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
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

void recordNoStderr(LaunchSettings& settings,
                    [[maybe_unused]] std::string_view value) {
  settings.output.toStderr = false;
}

std::optional<LaunchOption> findLaunchOption(std::string_view argument) {
  constexpr std::array launchOptions{
      LaunchOption{
          .name = LEVEL_OPTION, .takesValue = true, .record = recordLevel},
      LaunchOption{
          .name = FILE_OPTION, .takesValue = true, .record = recordFilePath},
      LaunchOption{
          .name = NO_FILE_OPTION, .takesValue = false, .record = recordNoFile},
      LaunchOption{
          .name = STDERR_OPTION, .takesValue = false, .record = recordStderr},
      LaunchOption{.name = NO_STDERR_OPTION,
                   .takesValue = false,
                   .record = recordNoStderr},
  };
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

/**
 * @returns the logging option @p arguments starts with; nothing when it is
 * empty or starts with another argument.
 * @throws InvalidLogOptionException when its first argument starts with
 * `--log-` or `--no-log-` but names no logging option.
 */
std::optional<LaunchOption> optionStarting(
    std::span<const std::string_view> arguments) {
  if (arguments.empty()) {
    return std::nullopt;
  }
  const std::optional<LaunchOption> option =
      findLaunchOption(arguments.front());
  if (!option.has_value() && isLoggingOption(arguments.front())) {
    throw InvalidLogOptionException(std::string(arguments.front()));
  }
  return option;
}

void appendUsageEntry(std::string& text, std::string_view option,
                      std::string_view description) {
  text += "  ";
  text += option;
  text += "\n      ";
  text += description;
  text += '\n';
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

/**
 * @returns @p journal made absolute from the current directory, or @p journal
 * itself when the current directory cannot be read.
 */
std::filesystem::path absoluteJournalPath(
    const std::filesystem::path& journal) {
  std::error_code error;
  std::filesystem::path absolute = std::filesystem::absolute(journal, error);
  if (error) {
    return journal;
  }
  return absolute;
}

/**
 * @brief Writes, at the info level, where the journal of @p output is, or
 * that there is none, so a reader of either output knows where the other
 * lines go.
 */
void announceJournal(const LogOutput& output) {
  const Logger logger{std::string{LOGGING_LOGGER_NAME}};
  if (!output.filePath.has_value()) {
    logger.info("no journal file");
    return;
  }
  logger.info("journal: ", absoluteJournalPath(*output.filePath).string());
}

}  // namespace

LogLaunchOptions::LogLaunchOptions(const std::filesystem::path& defaultJournal)
    : output_{.toStderr = true, .filePath = defaultJournal} {}

std::size_t LogLaunchOptions::readOption(
    std::span<const std::string_view> arguments) {
  const std::optional<LaunchOption> option = optionStarting(arguments);
  if (!option.has_value()) {
    return 0;
  }
  std::size_t index = 0;
  const std::string_view value =
      option->takesValue ? valueAfter(arguments, index) : std::string_view{};
  LaunchSettings settings{.level = level_, .output = output_};
  option->record(settings, value);
  level_ = settings.level;
  output_ = std::move(settings.output);
  return index + 1;
}

void LogLaunchOptions::apply() const {
  Logger::setOutput(output_);
  if (level_.has_value()) {
    Logger::setLevel(*level_);
  }
  announceJournal(output_);
}

std::string LogLaunchOptions::usage(std::string_view defaultJournalName) {
  std::string text = "Logging options:\n";
  appendUsageEntry(text, std::string(LEVEL_OPTION) + " <name>",
                   "Level: trace, debug, info, warn, error or silent. "
                   "Default: " +
                       std::string(LEVEL_ENVIRONMENT_VARIABLE) +
                       " if it names a level, otherwise " +
                       std::string(INFO_NAME) + ".");
  appendUsageEntry(
      text, std::string(FILE_OPTION) + " <path>",
      "Journal to append to. Default: " + std::string(defaultJournalName) +
          " next to the executable.");
  appendUsageEntry(text, NO_FILE_OPTION, "Write no journal.");
  appendUsageEntry(text, STDERR_OPTION,
                   "Write to standard error too. This is the default.");
  appendUsageEntry(text, NO_STDERR_OPTION, "Write nothing to standard error.");
  return text;
}

}  // namespace rtype::logging
