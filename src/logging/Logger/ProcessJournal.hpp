#pragma once

#include "ILogJournal.hpp"
#include "LogLevel.hpp"
#include "LogRecord.hpp"

namespace rtype::logging {

/**
 * @brief The journal of the process that owns it: it writes through the Logger
 * of its own copy of the library, so a line goes to the sinks and at the level
 * that Logger::setOutput() and Logger::setLevel() set in the program.
 */
class ProcessJournal final : public ILogJournal {
 public:
  /** @returns Whether Logger::shouldLog() accepts @p level. */
  [[nodiscard]] bool accepts(LogLevel level) const override;

  /** @brief Writes the record through a Logger called as the logger of the
   * record; a line of level Silent writes nothing. */
  void write(const LogRecord& record) const override;
};

}  // namespace rtype::logging
