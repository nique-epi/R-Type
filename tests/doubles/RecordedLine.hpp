#pragma once

#include <string>
#include "LogLevel.hpp"

/** @brief A line a RecordingJournal was asked to write: the level, the name of
 * the logger, the text that follows its prefix, and the JSON object of the
 * name and the values of an event (empty for a plain line). */
struct RecordedLine {
  rtype::logging::LogLevel level{rtype::logging::LogLevel::Info};
  std::string loggerName;
  std::string body;
  std::string fields;

  bool operator==(const RecordedLine&) const = default;
};
