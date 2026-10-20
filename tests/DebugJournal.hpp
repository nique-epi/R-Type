#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "JournalFile.hpp"
#include "Logger.hpp"

/**
 * @brief The log at the debug level, written to a JournalFile the test reads,
 * for the life of the object. The destructor puts the previous level back, and
 * the journal puts the log back on standard error.
 */
class DebugJournal {
 public:
  DebugJournal();
  ~DebugJournal();

  DebugJournal(const DebugJournal&) = delete;
  DebugJournal& operator=(const DebugJournal&) = delete;
  DebugJournal(DebugJournal&&) = delete;
  DebugJournal& operator=(DebugJournal&&) = delete;

  /** @returns The lines of the journal that hold @p text, oldest first. */
  [[nodiscard]] std::vector<std::string> linesHolding(
      std::string_view text) const;

 private:
  JournalFile journal_;
  rtype::logging::LogLevel savedLevel_;
};
