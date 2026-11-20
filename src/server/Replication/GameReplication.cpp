#include "GameReplication.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "GameContext.hpp"
#include "GameEndAnnouncer.hpp"
#include "GameOutcome.hpp"
#include "LobbyMap.hpp"
#include "LobbyStateAnnouncer.hpp"
#include "MapChoice.hpp"
#include "ServerConstants.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "ShipSpawnAnnouncerParts.hpp"
#include "StateSending.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @returns The maps the waiting room shows for @p maps, in their order: the
 * name and the preview of each. */
std::vector<network::LobbyMap> lobbyMapsOf(const std::vector<MapChoice>& maps) {
  std::vector<network::LobbyMap> shown;
  shown.reserve(maps.size());
  for (const MapChoice& map : maps) {
    shown.push_back(
        network::LobbyMap{.name = map.name, .preview = map.preview});
  }
  return shown;
}

}  // namespace

GameReplication::GameReplication(GameContext& game)
    : roster_(game.roster.get()),
      send_(game.send),
      ship_(game.settings.types.get().named(SHIP_SHEET_NAME)),
      lobbyMaps_(lobbyMapsOf(game.settings.maps)),
      worldStateSender_(
          roster_, send_,
          StateSending{.statesPerSecond = game.settings.statesPerSecond,
                       .encoding = game.settings.stateEncoding}) {}

void GameReplication::onWorldBuilt(game::World& world) {
  shipSpawnAnnouncer_ =
      std::make_unique<ShipSpawnAnnouncer>(ShipSpawnAnnouncerParts{
          .world = world, .roster = roster_, .send = send_, .ship = ship_});
  lobbyStateAnnouncer_ =
      std::make_unique<LobbyStateAnnouncer>(roster_, send_, lobbyMaps_);
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
