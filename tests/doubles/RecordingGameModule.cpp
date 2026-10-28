#include "RecordingGameModule.hpp"
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "EngineException.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "InstallContext.hpp"
#include "RecordingGameModuleConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

RecordingGameModule::RecordingGameModule(std::string name,
                                         std::vector<std::string>& journal,
                                         bool failsToStart)
    : name_(std::move(name)), journal_(&journal), failsToStart_(failsToStart) {}

std::string RecordingGameModule::name() const { return name_; }

void RecordingGameModule::install([[maybe_unused]] GameContext& game,
                                  engine::InstallContext& install) {
  journal_->push_back(name_ + " " + std::string{INSTALL_CALL});
  for (const std::string_view stage : GAME_STAGES) {
    install.addWork(stage, [this, stage]([[maybe_unused]] engine::Duration) {
      journal_->push_back(name_ + " " + std::string{stage});
    });
  }
}

void RecordingGameModule::start() {
  journal_->push_back(name_ + " " + std::string{START_CALL});
  if (failsToStart_) {
    throw engine::NullModuleException();
  }
}

void RecordingGameModule::stop() {
  journal_->push_back(name_ + " " + std::string{STOP_CALL});
}

}  // namespace rtype::server::testing
