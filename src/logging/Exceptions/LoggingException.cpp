#include "LoggingException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::logging {

LoggingException::LoggingException(const std::string& message)
    : std::runtime_error(message) {}

LogFileOpenException::LogFileOpenException(const std::string& filePath)
    : LoggingException("Cannot open the log file: " + filePath) {}

}  // namespace rtype::logging
