#pragma once

#include "JournalFile.hpp"
#include "LogLevel.hpp"

/**
 * @brief The log at the debug level, written as JSON Lines to a JournalFile
 * the test reads, for the life of the object. The destructor puts the previous
 * level back, and the journal puts the log back on standard error.
 */
class JsonJournal {
 public:
  JsonJournal();
  ~JsonJournal();

  JsonJournal(const JsonJournal&) = delete;
  JsonJournal& operator=(const JsonJournal&) = delete;
  JsonJournal(JsonJournal&&) = delete;
  JsonJournal& operator=(JsonJournal&&) = delete;

  /** @returns The journal file, whose events() the test reads. */
  [[nodiscard]] const JournalFile& file() const;

 private:
  JournalFile journal_;
  rtype::logging::LogLevel savedLevel_;
};
