#pragma once

#include <stdexcept>
#include <string>

namespace rtype::logging {

/**
 * @brief Root of every error raised by the logging module.
 */
class LoggingException : public std::runtime_error {
 public:
  explicit LoggingException(const std::string& message);
};

/**
 * @brief A log level name that matches none of the known levels.
 */
class UnknownLogLevelException : public LoggingException {
 public:
  explicit UnknownLogLevelException(const std::string& levelName);
};

/**
 * @brief A logging option that this program does not define.
 */
class InvalidLogOptionException : public LoggingException {
 public:
  explicit InvalidLogOptionException(const std::string& option);
};

/**
 * @brief A logging option that needs a value was the last argument.
 */
class MissingLogOptionValueException : public LoggingException {
 public:
  explicit MissingLogOptionValueException(const std::string& option);
};

/**
 * @brief The log file could not be opened for appending.
 */
class LogFileOpenException : public LoggingException {
 public:
  explicit LogFileOpenException(const std::string& filePath);
};

}  // namespace rtype::logging
