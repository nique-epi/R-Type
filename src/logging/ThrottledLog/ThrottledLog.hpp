#pragma once

#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include "Logger.hpp"
#include "LoggingConstants.hpp"

namespace rtype::logging {

/**
 * @brief The log lines of an event that can repeat as fast as someone causes
 * it, such as a datagram the server drops: the first event gets a line, and
 * the next line goes to the first event at least one interval after the
 * previous line, so a flood writes one line per interval whatever its rate.
 *
 * Every event is counted, and each line ends with how many events happened
 * since the previous line, this one included, so the events without a line
 * still show in the next one. The first line counts every event since the log
 * was built. Not thread-safe: use one per thread.
 */
class ThrottledLog {
 public:
  /**
   * @param module The module name of the lines, as Logger takes it.
   * @param interval The shortest time between two lines.
   */
  ThrottledLog(std::string module, std::chrono::nanoseconds interval);

  /**
   * @brief Counts one event that happened at @p now and, when it gets a line,
   * writes @p args at the debug level followed by the count.
   * @param now Measured from any fixed origin, the same for every call, and
   * never earlier than the previous call.
   */
  template <class... Args>
  void debug(std::chrono::nanoseconds now, Args&&... args) {
    if (const std::optional<std::size_t> count = countEvent(now)) {
      logger_.debug(std::forward<Args>(args)..., EVENT_COUNT_OPENING, *count,
                    EVENT_COUNT_CLOSING);
    }
  }

 private:
  /**
   * @returns How many events were counted since the previous line, the one at
   * @p now included, when it gets a line; nothing when it does not.
   */
  [[nodiscard]] std::optional<std::size_t> countEvent(
      std::chrono::nanoseconds now);

  Logger logger_;
  std::chrono::nanoseconds interval_;
  std::optional<std::chrono::nanoseconds> previousLineTime_;
  std::size_t eventsSincePreviousLine_{0};
};

}  // namespace rtype::logging
