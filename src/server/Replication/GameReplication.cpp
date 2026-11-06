#include "GameReplication.hpp"
#include <cstdint>
#include <memory>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "GameContext.hpp"
#include "GameEndAnnouncer.hpp"
#include "GameOutcome.hpp"
#include "LobbyStateAnnouncer.hpp"
#include "ServerConstants.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "ShipSpawnAnnouncerParts.hpp"
#include "StateSending.hpp"
#include "World.hpp"

namespace rtype::server {

GameReplication::GameReplication(GameContext& game)
    : roster_(game.roster.get()),
      send_(game.send),
      ship_(game.settings.types.get().named(SHIP_SHEET_NAME)),
      worldStateSender_(
          roster_, send_,
          StateSending{.statesPerSecond = game.settings.statesPerSecond,
                       .encoding = game.settings.stateEncoding}) {}

void GameReplication::onWorldBuilt(game::World& world) {
  shipSpawnAnnouncer_ =
      std::make_unique<ShipSpawnAnnouncer>(ShipSpawnAnnouncerParts{
          .world = world, .roster = roster_, .send = send_, .ship = ship_});
  lobbyStateAnnouncer_ = std::make_unique<LobbyStateAnnouncer>(roster_, send_);
  gameEndAnnouncer_.reset();
}

void GameReplication::onPlayerJoined(ConnectedPlayer& player) {
  shipSpawnAnnouncer_->onPlayerJoined(player);
}

void GameReplication::onWaitingTick() { lobbyStateAnnouncer_->onWaitingTick(); }

void GameReplication::onGameFinished(GameOutcome outcome) {
  gameEndAnnouncer_ =
      std::make_unique<GameEndAnnouncer>(roster_, send_, outcome);
}

void GameReplication::onFinishedTick() {
  if (gameEndAnnouncer_ != nullptr) {
    gameEndAnnouncer_->onFinishedTick();
  }
}

void GameReplication::onProgressTick(std::uint32_t tick,
                                     engine::ComponentRegistry& components) {
  worldStateSender_.onTick(tick, components);
}

}  // namespace rtype::server
