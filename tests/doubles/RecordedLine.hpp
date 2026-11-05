#pragma once

#include <string>
#include "LogLevel.hpp"

/** @brief A line a RecordingJournal was asked to write: the level, the name of
 * the logger and the text that follows its prefix. */
struct RecordedLine {
  rtype::logging::LogLevel level{rtype::logging::LogLevel::Info};
  std::string loggerName;
  std::string body;

  bool operator==(const RecordedLine&) const = default;
};
