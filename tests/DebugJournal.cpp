#include "DebugJournal.hpp"
#include <string>
#include <string_view>
#include <vector>
#include "JournalFile.hpp"
#include "LogFormat.hpp"
#include "LogLevel.hpp"
#include "Logger.hpp"
#include "TestJournalConstants.hpp"

DebugJournal::DebugJournal(rtype::logging::LogFormat format)
    : journal_(format == rtype::logging::LogFormat::Json ? JSON_JOURNAL_SUFFIX
                                                         : std::string_view{}),
      savedLevel_(rtype::logging::Logger::level()) {
  rtype::logging::Logger::setLevel(rtype::logging::LogLevel::Debug);
  rtype::logging::Logger::setOutput(
      rtype::logging::LogOutput{.toStderr = false,
                                .filePath = journal_.path(),
                                .forceColor = false,
                                .fileFormat = format});
}

DebugJournal::~DebugJournal() { rtype::logging::Logger::setLevel(savedLevel_); }

std::vector<std::string> DebugJournal::linesHolding(
    std::string_view text) const {
  return journal_.linesHolding(text);
}

const JournalFile& DebugJournal::file() const { return journal_; }
