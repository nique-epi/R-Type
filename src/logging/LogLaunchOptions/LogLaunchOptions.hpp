#pragma once

#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
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
   * @brief The options of a launch that gives no logging option: the journal
   *        is @p defaultJournal and standard error is on, so a program
   *        launched from a terminal shows its lines there.
   */
  explicit LogLaunchOptions(const std::filesystem::path& defaultJournal);

  /**
   * @brief Reads the logging option @p arguments starts with, if it is one:
   *        `--log-level <name>`, `--log-file <path>`, `--no-log-file`,
   *        `--log-stderr` or `--no-log-stderr`.
   *
   * The value of `--log-level` or `--log-file` is the argument after it, even
   * when it looks like an option. A program reads its arguments once, in
   * order, and hands this the ones that are neither its own options nor the
   * value of one, so the last of `--log-file` and `--no-log-file` wins, and
   * so does the last of `--log-stderr` and `--no-log-stderr`. A relative path
   * after `--log-file` is read from the folder the program was started in.
   *
   * @returns how many arguments the option took: 2 for an option and its
   *          value, 1 for an option alone, and 0 when @p arguments is empty
   *          or starts with another argument, which reads nothing.
   * @throws UnknownLogLevelException when the value of `--log-level` names no
   *         level.
   * @throws InvalidLogOptionException when the first argument starts with
   *         `--log-` or `--no-log-` but names no logging option, so a typo is
   *         never silent.
   * @throws MissingLogOptionValueException when `--log-level` or
   *         `--log-file` is the last argument.
   */
  [[nodiscard]] std::size_t readOption(
      std::span<const std::string_view> arguments);

  /**
   * @brief Configures the Logger with the options read, then writes at the
   *        info level, under the module LOGGING_LOGGER_NAME, the absolute path
   *        of the journal, or that there is none.
   *
   * @throws LogFileOpenException when the journal cannot be opened. The level
   *         and the output of the Logger are then unchanged, and nothing is
   *         written.
   */
  void apply() const;

  /**
   * @returns the part of a program's usage that describes the logging
   *          options: each option on a line, its effect indented on the next.
   * @param defaultJournalName the name of the journal the program writes next
   *        to its executable unless an option changes it.
   */
  [[nodiscard]] static std::string usage(std::string_view defaultJournalName);

 private:
  std::optional<LogLevel> level_;
  LogOutput output_;
};

}  // namespace rtype::logging
