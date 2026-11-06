#include "ChatModule.hpp"
#include <memory>
#include <string>
#include "ChatLine.hpp"
#include "ChatRelay.hpp"
#include "ConnectedPlayer.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "InstallContext.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

std::string ChatModule::name() const { return std::string{CHAT_MODULE_NAME}; }

void ChatModule::install(GameContext& game, engine::InstallContext& install) {
  game_ = &game;
  install.addWork(
      GAME_SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { relay_->onTick(); });
}

void ChatModule::start() {
  relay_ = std::make_unique<ChatRelay>(game_->roster, game_->send);
  game_->hooks.get().closing.emplace_back(
      [this](const ConnectedPlayer& departed) {
        relay_->onPlayerClosing(departed);
      });
  game_->hooks.get().chatLineSent.emplace_back(
      [this](const ConnectedPlayer& player, const network::ChatLine& line) {
        relay_->onLine(player, line);
      });
}

void ChatModule::stop() { relay_.reset(); }

}  // namespace rtype::server
