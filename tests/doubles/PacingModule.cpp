#include "PacingModule.hpp"
#include <string>
#include <vector>
#include "IStageRunner.hpp"
#include "InstallContext.hpp"
#include "PacingModuleConstants.hpp"
#include "TimeConstants.hpp"

std::string PacingModule::name() const { return PACING_MODULE; }

std::vector<std::string> PacingModule::dependencies() const { return {}; }

void PacingModule::install(rtype::engine::InstallContext& context) {
  stages_ = &context.require<rtype::engine::IStageRunner>();
}

void PacingModule::start() {}

void PacingModule::stop() {}

void PacingModule::runOnePass(rtype::engine::Duration elapsed) const {
  stages_->runStages(elapsed);
}
