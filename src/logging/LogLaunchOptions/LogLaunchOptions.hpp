#pragma once

#include <span>
#include <string_view>

namespace rtype::logging {

/**
 * @brief Applies the logging options of a program launch to the Logger.
 */
class LogLaunchOptions {
 public:
  /**
   * @brief Reads `--log-level <name>`, `--log-file <path>`, `--no-log-file`
   *        and `--log-stderr`, then configures the Logger.
   *
   * Without options the journal is @p defaultFileName and standard error is
   * off. The last of `--log-file` and `--no-log-file` wins. Arguments that do
   * not start with `--log-` or `--no-log-` are ignored. Every option is read
   * before the Logger is touched, so nothing is changed when an error is
   * raised.
   *
   * @param arguments the arguments after the program name.
   * @param defaultFileName the journal used unless an option changes it.
   * @throws UnknownLogLevelException, InvalidLogOptionException,
   *         MissingLogOptionValueException, LogFileOpenException
   */
  static void apply(std::span<const std::string_view> arguments,
                    std::string_view defaultFileName);

  /**
   * @brief Same as apply() for the running process: the program name, the
   *        first argument, is skipped.
   */
  static void applyFromProcess(int argumentCount, const char* const* arguments,
                               std::string_view defaultFileName);
};

}  // namespace rtype::logging
