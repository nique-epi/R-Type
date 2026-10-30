#include "GameReplication.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EntitySpawnSender.hpp"
#include "GameEndAnnouncer.hpp"
#include "LobbyStateAnnouncer.hpp"
#include "PlayerRoster.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "World.hpp"

namespace rtype::server {

GameReplication::GameReplication(PlayerRoster& roster, SendDatagram send,
                                 std::size_t statesPerSecond)
    : roster_(roster),
      send_(std::move(send)),
      worldStateSender_(roster_, send_, statesPerSecond) {}

void GameReplication::onWorldBuilt(game::World& world) {
  shipSpawnAnnouncer_ =
      std::make_unique<ShipSpawnAnnouncer>(world, roster_, send_);
  lobbyStateAnnouncer_ = std::make_unique<LobbyStateAnnouncer>(roster_, send_);
  gameEndAnnouncer_.reset();
}

void GameReplication::onPlayerJoined(ConnectedPlayer& player) {
  shipSpawnAnnouncer_->onPlayerJoined(player);
}

void GameReplication::onWaitingTick() { lobbyStateAnnouncer_->onWaitingTick(); }

void GameReplication::onGameFinished() {
  gameEndAnnouncer_ = std::make_unique<GameEndAnnouncer>(roster_, send_);
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
