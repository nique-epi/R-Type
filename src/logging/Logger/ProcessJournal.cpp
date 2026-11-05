#include "ProcessJournal.hpp"
#include <string>
#include <string_view>
#include "LogLevel.hpp"
#include "LogRecord.hpp"
#include "Logger.hpp"

namespace rtype::logging {

bool ProcessJournal::accepts(LogLevel level) const {
  return Logger::shouldLog(level);
}

void ProcessJournal::write(const LogRecord& record) const {
  const Logger logger{std::string{record.loggerName}};
  const std::string_view body = record.body;
  switch (record.level) {
    case LogLevel::Trace:
      logger.trace(body);
      break;
    case LogLevel::Debug:
      logger.debug(body);
      break;
    case LogLevel::Info:
      logger.info(body);
      break;
    case LogLevel::Warn:
      logger.warn(body);
      break;
    case LogLevel::Error:
      logger.error(body);
      break;
    case LogLevel::Silent:
      break;
  }
}

}  // namespace rtype::logging
