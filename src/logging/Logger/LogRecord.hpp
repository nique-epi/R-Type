#pragma once

#include <string_view>
#include "LogLevel.hpp"

namespace rtype::logging {

/**
 * @brief One line a logger asks a journal to write: its level, the name of the
 * logger and the text that follows the prefix of the line. It points into
 * strings the logger owns, valid for the call that receives it only.
 */
struct LogRecord {
  LogLevel level{LogLevel::Info};
  std::string_view loggerName;
  std::string_view body;
};

}  // namespace rtype::logging
