#include "LogJournalProbeModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "ILogJournal.hpp"
#include "InstallContext.hpp"

LogJournalProbeModule::LogJournalProbeModule(
    std::string name, std::vector<std::string> dependencies)
    : name_(std::move(name)), dependencies_(std::move(dependencies)) {}

std::string LogJournalProbeModule::name() const { return name_; }

std::vector<std::string> LogJournalProbeModule::dependencies() const {
  return dependencies_;
}

void LogJournalProbeModule::install(rtype::engine::InstallContext& context) {
  journal_ = context.find<rtype::logging::ILogJournal>();
}

void LogJournalProbeModule::start() {}

void LogJournalProbeModule::stop() {}

rtype::logging::ILogJournal* LogJournalProbeModule::journal() const {
  return journal_;
}
