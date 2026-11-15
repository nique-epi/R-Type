#include "JsonJournal.hpp"
#include "JournalFile.hpp"
#include "LogFormat.hpp"
#include "LogLevel.hpp"
#include "Logger.hpp"

JsonJournal::JsonJournal() : savedLevel_(rtype::logging::Logger::level()) {
  rtype::logging::Logger::setLevel(rtype::logging::LogLevel::Debug);
  rtype::logging::Logger::setOutput(
      rtype::logging::LogOutput{.toStderr = false,
                                .filePath = journal_.path(),
                                .forceColor = false,
                                .fileFormat = rtype::logging::LogFormat::Json});
}

JsonJournal::~JsonJournal() { rtype::logging::Logger::setLevel(savedLevel_); }

const JournalFile& JsonJournal::file() const { return journal_; }
