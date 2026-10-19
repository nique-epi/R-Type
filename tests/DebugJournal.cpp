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
  std::vector<std::string> found;
  for (const std::string& line : journal_.lines()) {
    if (line.find(text) != std::string::npos) {
      found.push_back(line);
    }
  }
  return found;
}
