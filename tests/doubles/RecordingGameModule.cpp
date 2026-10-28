#include "RecordingGameModule.hpp"
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "InstallContext.hpp"
#include "RecordingGameModuleConstants.hpp"
#include "SessionHooks.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::string journalLine(std::string_view module, std::string_view what) {
  std::string line{module};
  line += ' ';
  line += what;
  return line;
}

RecordingGameModule::RecordingGameModule(std::string name,
                                         std::vector<std::string>& journal,
                                         bool failsToStart)
    : name_(std::move(name)), journal_(&journal), failsToStart_(failsToStart) {}

std::string RecordingGameModule::name() const { return name_; }

void RecordingGameModule::install(GameContext& game,
                                  engine::InstallContext& install) {
  game_ = &game;
  journal_->push_back(journalLine(name_, INSTALL_CALL));
  for (const std::string_view stage : GAME_STAGES) {
    install.addWork(stage, [this, stage]([[maybe_unused]] engine::Duration) {
      journal_->push_back(journalLine(name_, stage));
    });
  }
}

void RecordingGameModule::start() {
  journal_->push_back(journalLine(name_, START_CALL));
  if (failsToStart_) {
    throw engine::NullModuleException();
  }
  SessionHooks& hooks = game_->hooks;
  hooks.joined.emplace_back([this](ConnectedPlayer&) {
    journal_->push_back(journalLine(name_, JOINED_CALL));
  });
  hooks.closing.emplace_back([this](const ConnectedPlayer&) {
    journal_->push_back(journalLine(name_, LEAVING_CALL));
  });
}

void RecordingGameModule::stop() {
  journal_->push_back(journalLine(name_, STOP_CALL));
}

}  // namespace rtype::server::testing
