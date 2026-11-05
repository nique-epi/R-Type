#include "RecordingJournal.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "LogLevel.hpp"
#include "LogRecord.hpp"
#include "RecordedLine.hpp"

RecordingJournal::RecordingJournal(rtype::logging::LogLevel minimum)
    : minimum_(minimum) {}

bool RecordingJournal::accepts(rtype::logging::LogLevel level) const {
  return static_cast<std::uint8_t>(level) >=
         static_cast<std::uint8_t>(minimum_);
}

void RecordingJournal::write(const rtype::logging::LogRecord& record) const {
  lines_.push_back(RecordedLine{.level = record.level,
                                .loggerName = std::string{record.loggerName},
                                .body = std::string{record.body}});
}

const std::vector<RecordedLine>& RecordingJournal::lines() const {
  return lines_;
}
