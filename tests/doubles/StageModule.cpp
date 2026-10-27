#include "StageModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"
#include "JournalModule.hpp"
#include "StageModuleConstants.hpp"
#include "StageModuleSetup.hpp"
#include "TimeConstants.hpp"

StageModule::StageModule(StageModuleSetup setup, JournalModule& journal)
    : setup_(std::move(setup)), journal_(&journal) {}

std::string StageModule::name() const { return setup_.name; }

std::vector<std::string> StageModule::dependencies() const { return {}; }

void StageModule::install(rtype::engine::InstallContext& context) {
  context.addWork(setup_.stage,
                  [this]([[maybe_unused]] rtype::engine::Duration elapsed) {
                    recordRun();
                  });
}

void StageModule::start() {}

void StageModule::stop() {}

void StageModule::recordRun() {
  journal_->calls.push_back(setup_.stage + STAGE_RUN_SEPARATOR + setup_.name);
}
