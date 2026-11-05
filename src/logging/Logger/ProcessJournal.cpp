#include "ProcessJournal.hpp"
#include "LogLevel.hpp"
#include "LogRecord.hpp"
#include "Logger.hpp"

namespace rtype::logging {

bool ProcessJournal::accepts(LogLevel level) const {
  return Logger::shouldLog(level);
}

void ProcessJournal::write(const LogRecord& record) const {
  if (record.level != LogLevel::Silent) {
    writeLogLine(record);
  }
}

}  // namespace rtype::logging
