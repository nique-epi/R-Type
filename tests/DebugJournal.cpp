#include "DebugJournal.hpp"
#include <string>
#include <string_view>
#include <vector>
#include "Logger.hpp"

DebugJournal::DebugJournal() : savedLevel_(rtype::logging::Logger::level()) {
  rtype::logging::Logger::setLevel(rtype::logging::LogLevel::Debug);
  rtype::logging::Logger::setOutput(rtype::logging::LogOutput{
      .toStderr = false, .filePath = journal_.path()});
}

DebugJournal::~DebugJournal() { rtype::logging::Logger::setLevel(savedLevel_); }

std::vector<std::string> DebugJournal::linesHolding(
    std::string_view text) const {
  return journal_.linesHolding(text);
}
