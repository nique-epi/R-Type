#pragma once

#include "LogLevel.hpp"
#include "LogRecord.hpp"

namespace rtype::logging {

/**
 * @brief The journal of the program: its sinks and its level, reached through
 * virtual calls. A plugin holds its own copy of Logger's static state, which
 * the program never configures; a Logger built on this journal writes where the
 * lines of the program go, at the level of the program.
 */
class ILogJournal {
 public:
  virtual ~ILogJournal() = default;

  /** @returns Whether a line of @p level is written. */
  [[nodiscard]] virtual bool accepts(LogLevel level) const = 0;

  /** @brief Writes @p record as a line. */
  virtual void write(const LogRecord& record) const = 0;
};

}  // namespace rtype::logging
