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
 * @brief The log file could not be opened for appending.
 */
class LogFileOpenException : public LoggingException {
 public:
  explicit LogFileOpenException(const std::string& filePath);
};

}  // namespace rtype::logging
