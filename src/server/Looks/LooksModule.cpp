#include "LooksModule.hpp"
#include <memory>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "InstallContext.hpp"
#include "ShipLookChoice.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

std::string LooksModule::name() const { return std::string{LOOKS_MODULE_NAME}; }

void LooksModule::install(GameContext& game, engine::InstallContext& install) {
  game_ = &game;
  install.addWork(
      GAME_SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { relay_->onTick(); });
}

void LooksModule::start() {
  relay_ = std::make_unique<LookRelay>(game_->roster, game_->send);
  game_->hooks.get().joined.emplace_back(
      [this]([[maybe_unused]] ConnectedPlayer& player) {
        relay_->onPlayerJoined();
      });
  game_->hooks.get().closing.emplace_back(
      [this](const ConnectedPlayer& departed) {
        relay_->onPlayerClosing(departed);
      });
  game_->hooks.get().shipLookChosen.emplace_back(
      [this](ConnectedPlayer& player, const network::ShipLookChoice& choice) {
        relay_->onChoice(player, choice);
      });
}

void LooksModule::stop() { relay_.reset(); }

}  // namespace rtype::server
