#include "CasinoModule.hpp"
#include <memory>
#include <string>
#include <utility>
#include "CasinoConstants.hpp"
#include "CasinoPlace.hpp"
#include "ConnectedPlayer.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "GameState.hpp"
#include "GameStateChanged.hpp"
#include "IRouletteDraw.hpp"
#include "InstallContext.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "RouletteRoundAnnouncer.hpp"
#include "RouletteTable.hpp"
#include "SystemRouletteDraw.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

CasinoModule::CasinoModule()
    : CasinoModule(std::make_unique<network::SystemRouletteDraw>()) {}

CasinoModule::CasinoModule(std::unique_ptr<network::IRouletteDraw> draw)
    : draw_(std::move(draw)), logger_(std::string{CASINO_LOGGER_NAME}) {}

std::string CasinoModule::name() const {
  return std::string{CASINO_MODULE_NAME};
}

void CasinoModule::install(GameContext& game, engine::InstallContext& install) {
  game_ = &game;
  install.addWork(
      GAME_SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { onTick(); });
}

void CasinoModule::start() {
  table_ = std::make_unique<RouletteTable>(game_->roster, *draw_);
  announcer_ =
      std::make_unique<RouletteRoundAnnouncer>(game_->roster, game_->send);
  stateSubscription_ = game_->events.get().subscribe<GameStateChanged>(
      [this](const GameStateChanged& change) {
        open_ = change.current == GameState::WaitingForPlayers;
        if (!open_) {
          table_->close();
          announcer_->forget();
        }
      });
  game_->hooks.get().closing.emplace_back(
      [this](const ConnectedPlayer& departed) {
        table_->onPlayerClosing(departed);
      });
  game_->hooks.get().casinoPlaceChosen.emplace_back(
      [this](ConnectedPlayer& player, const network::CasinoPlace& place) {
        table_->onPlace(player, place.place);
      });
  game_->hooks.get().rouletteBetsSent.emplace_back(
      [this](const ConnectedPlayer& player, const network::RouletteBets& bets) {
        table_->onBets(player, bets);
      });
}

void CasinoModule::stop() {
  static_cast<void>(game_->events.get().unsubscribe(stateSubscription_));
  announcer_.reset();
  table_.reset();
}

void CasinoModule::onTick() {
  if (!open_) {
    return;
  }
  table_->onTick();
  const network::RouletteRound round = table_->round();
  if (round.phase == network::RoulettePhase::Spinning &&
      shownPhase_ == network::RoulettePhase::Betting) {
    logger_.info("round ", round.round, " drew ",
                 static_cast<unsigned int>(round.winningNumber));
  }
  shownPhase_ = round.phase;
  announcer_->onTick(round);
}

}  // namespace rtype::server
