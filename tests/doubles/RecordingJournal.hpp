#pragma once

#include <vector>
#include "ILogJournal.hpp"
#include "LogLevel.hpp"
#include "LogRecord.hpp"
#include "RecordedLine.hpp"

/** @brief Journal double that keeps the lines it is asked to write, and accepts
 * the lines of a level at or above the minimum the test gives it. */
class RecordingJournal final : public rtype::logging::ILogJournal {
 public:
  /** @param minimum The lowest level the journal accepts; every level by
   * default. */
  explicit RecordingJournal(
      rtype::logging::LogLevel minimum = rtype::logging::LogLevel::Trace);

  [[nodiscard]] bool accepts(rtype::logging::LogLevel level) const override;
  void write(const rtype::logging::LogRecord& record) const override;

  /** @returns The lines written so far, in order. */
  [[nodiscard]] const std::vector<RecordedLine>& lines() const;

 private:
  rtype::logging::LogLevel minimum_;
  mutable std::vector<RecordedLine> lines_;
};
