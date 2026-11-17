#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "JournalFile.hpp"
#include "LogFormat.hpp"
#include "LogLevel.hpp"

/**
 * @brief The log at the debug level, written to a JournalFile the test reads,
 * for the life of the object, as text or, given LogFormat::Json, as JSON Lines
 * in a file of its own, so it may sit beside a text one. The destructor puts
 * the previous level back, and the journal puts the log back on standard
 * error.
 */
class DebugJournal {
 public:
  explicit DebugJournal(
      rtype::logging::LogFormat format = rtype::logging::LogFormat::Text);
  ~DebugJournal();

  DebugJournal(const DebugJournal&) = delete;
  DebugJournal& operator=(const DebugJournal&) = delete;
  DebugJournal(DebugJournal&&) = delete;
  DebugJournal& operator=(DebugJournal&&) = delete;

  /** @returns The lines of the journal that hold @p text, oldest first. */
  [[nodiscard]] std::vector<std::string> linesHolding(
      std::string_view text) const;

  /** @returns The journal file, whose events() a test reads. */
  [[nodiscard]] const JournalFile& file() const;

 private:
  JournalFile journal_;
  rtype::logging::LogLevel savedLevel_;
};
