#include "ResultsModule.hpp"
#include <memory>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "GameResultKeeper.hpp"
#include "IGameFlow.hpp"
#include "IResultsArchive.hpp"
#include "InstallContext.hpp"
#include "LeaderboardDesk.hpp"
#include "LeaderboardRequest.hpp"
#include "MessageOrigin.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

ResultsModule::ResultsModule(IResultsArchive& archive) : archive_(archive) {}

std::string ResultsModule::name() const {
  return std::string{RESULTS_MODULE_NAME};
}

void ResultsModule::install(GameContext& game,
                            engine::InstallContext& install) {
  game_ = &game;
  flow_ = &install.require<IGameFlow>();
  install.addWork(GAME_SIMULATION_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    keeper_->followState(flow_->state());
                    desk_->onTick();
                  });
}

void ResultsModule::start() {
  keeper_ = std::make_unique<GameResultKeeper>(game_->roster, archive_,
                                               game_->settings.difficulty);
  desk_ =
      std::make_unique<LeaderboardDesk>(game_->roster, archive_, game_->send);
  game_->hooks.get().closing.emplace_back(
      [this](const ConnectedPlayer& departed) {
        keeper_->onPlayerClosing(departed);
      });
  game_->messages.get().registerHandler<network::LeaderboardRequest>(
      [this](const network::MessageOrigin& origin,
             const network::LeaderboardRequest& request) {
        const ConnectedPlayer* const player =
            game_->roster.get().find(origin.sender);
        if (player != nullptr) {
          desk_->onAsked(*player, request);
        }
      });
}

void ResultsModule::stop() {
  desk_.reset();
  keeper_.reset();
}

}  // namespace rtype::server
