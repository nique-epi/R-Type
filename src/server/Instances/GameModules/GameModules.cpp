#include "GameModules.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "IGameModule.hpp"
#include "InstallContext.hpp"
#include "RequireNoNullModule.hpp"
#include "StageWork.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

GameModules::GameModules(std::vector<std::unique_ptr<IGameModule>> modules)
    : stages_(GAME_STAGES),
      game_(std::string{GAME_INSTALL_NAME}, services_, stages_),
      modules_(std::move(modules)) {
  engine::requireNoNullModule<IGameModule>(modules_);
}

GameModules::~GameModules() {
  while (startedCount_ > 0) {
    --startedCount_;
    modules_[startedCount_]->stop();
  }
}

void GameModules::addWork(std::string_view stageName, engine::StageWork work) {
  game_.addWork(stageName, std::move(work));
}

void GameModules::installAndStart(GameContext& game) {
  for (const std::unique_ptr<IGameModule>& listedModule : modules_) {
    engine::InstallContext installation(listedModule->name(), services_,
                                        stages_);
    listedModule->install(game, installation);
  }
  for (const std::unique_ptr<IGameModule>& listedModule : modules_) {
    listedModule->start();
    ++startedCount_;
  }
}

void GameModules::runStages(engine::Duration elapsed) { stages_.run(elapsed); }

}  // namespace rtype::server
