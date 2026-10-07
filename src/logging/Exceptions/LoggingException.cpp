#include "LoggingException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::logging {

LoggingException::LoggingException(const std::string& message)
    : std::runtime_error(message) {}

UnknownLogLevelException::UnknownLogLevelException(const std::string& levelName)
    : LoggingException("Unknown log level: " + levelName) {}

InvalidLogOptionException::InvalidLogOptionException(const std::string& option)
    : LoggingException("Unknown logging option: " + option) {}

MissingLogOptionValueException::MissingLogOptionValueException(
    const std::string& option)
    : LoggingException("Missing value after the logging option: " + option) {}

LogFileOpenException::LogFileOpenException(const std::string& filePath)
    : LoggingException("Cannot open the log file: " + filePath) {}

}  // namespace rtype::logging
