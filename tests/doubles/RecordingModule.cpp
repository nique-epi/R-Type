#include "RecordingModule.hpp"
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ModuleJournal.hpp"
#include "RecordingModuleConstants.hpp"
#include "RecordingModuleSetup.hpp"
#include "StartRefusedException.hpp"

RecordingModule::RecordingModule(RecordingModuleSetup setup,
                                 ModuleJournal& journal)
    : setup_(std::move(setup)), journal_(&journal) {}

std::string RecordingModule::name() const { return setup_.name; }

std::vector<std::string> RecordingModule::requiredModules() const {
  return setup_.requiredModules;
}

void RecordingModule::install() { record(INSTALL_CALL); }

void RecordingModule::start() {
  record(START_CALL);
  if (setup_.failsToStart) {
    throw StartRefusedException();
  }
}

void RecordingModule::stop() { record(STOP_CALL); }

void RecordingModule::record(std::string_view call) {
  journal_->calls.push_back(std::string(call) + setup_.name);
}
