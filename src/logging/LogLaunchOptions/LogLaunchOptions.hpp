#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "Logger.hpp"

namespace rtype::logging {

/**
 * @brief The logging options of a program launch, read from its arguments
 *        and applied to the Logger in a second step.
 *
 * Reading never touches the Logger, so a program can check all its options,
 * or print its usage, before the journal is opened.
 */
class LogLaunchOptions {
 public:
  /**
   * @brief Reads `--log-level <name>`, `--log-file <path>`, `--no-log-file`
   *        and `--log-stderr` from @p arguments.
   *
   * Without options the journal is @p defaultFileName and standard error is
   * off. The last of `--log-file` and `--no-log-file` wins. Arguments that do
   * not start with `--log-` or `--no-log-` are kept for the program, see
   * otherArguments().
   *
   * @param arguments the arguments after the program name. The options keep
   *        views on these strings: they must outlive the options.
   * @param defaultFileName the journal used unless an option changes it.
   * @throws UnknownLogLevelException, InvalidLogOptionException,
   *         MissingLogOptionValueException
   */
  [[nodiscard]] static LogLaunchOptions read(
      std::span<const std::string_view> arguments,
      std::string_view defaultFileName);

  /**
   * @brief Same as read() for the running process: the program name, the
   *        first argument, is skipped.
   */
  [[nodiscard]] static LogLaunchOptions readFromProcess(
      int argumentCount, const char* const* arguments,
      std::string_view defaultFileName);

  /**
   * @brief Configures the Logger with the options read.
   *
   * @throws LogFileOpenException when the journal cannot be opened. The level
   *         and the output of the Logger are then unchanged.
   */
  void apply() const;

  /**
   * @returns the arguments that are not logging options, in the order they
   *          were given, for the program to read.
   */
  [[nodiscard]] std::span<const std::string_view> otherArguments() const;

  /**
   * @returns the part of a program's usage that describes the logging
   *          options: each option on a line, its effect indented on the next.
   * @param defaultFileName the journal used unless an option changes it.
   */
  [[nodiscard]] static std::string usage(std::string_view defaultFileName);

 private:
  LogLaunchOptions(std::optional<LogLevel> level, LogOutput output,
                   std::vector<std::string_view> otherArguments);

  std::optional<LogLevel> level_;
  LogOutput output_;
  std::vector<std::string_view> otherArguments_;
};

}  // namespace rtype::logging
