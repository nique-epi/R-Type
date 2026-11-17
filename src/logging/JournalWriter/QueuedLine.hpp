#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "LogLevel.hpp"

namespace rtype::logging {

/**
 * @brief A line as the thread of the journal hands it over: its level, the name
 * of the logger that wrote it, its text, the time of the call that queued it,
 * and how many lines the full queue dropped since the line before it. The
 * views point into the queue, valid for the call that receives it only.
 */
struct QueuedLine {
  LogLevel level{LogLevel::Info};
  std::string_view loggerName;
  std::string_view text;
  std::chrono::system_clock::time_point time;
  std::size_t lostBefore{0};
};

}  // namespace rtype::logging
