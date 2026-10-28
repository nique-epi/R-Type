#include "GameInstance.hpp"
#include <cstdint>
#include <span>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "GameFlowParts.hpp"
#include "GameSessionsParts.hpp"
#include "GameSettings.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

GameInstance::GameInstance(Send send, GameSettings settings,
                           engine::EventBus& events)
    : send_(std::move(send)),
      events_(events),
      sessions_(GameSessionsParts{
          .events = events,
          .send = send_,
          .readyAccepted =
              [this] { return flow_.state() == GameState::WaitingForPlayers; },
          .onClosing =
              [this](const ConnectedPlayer& departed) {
                removeShipOf(departed);
              }}),
      scoreKeeper_(events, sessions_.roster()),
      flow_(GameFlowParts{.events = events,
                          .roster = sessions_.roster(),
                          .send = send_,
                          .everyPlayerIsReady =
                              [this] { return sessions_.everyPlayerIsReady(); },
                          .settings = std::move(settings)}) {}

GameState GameInstance::state() const { return flow_.state(); }

std::uint32_t GameInstance::tick() const { return flow_.tick(); }

PlayerRoster& GameInstance::roster() { return sessions_.roster(); }

game::World& GameInstance::world() { return flow_.world(); }

network::PacketHeader GameInstance::nextSessionHeaderFor(
    const network::Endpoint& player) {
  return sessions_.nextSessionHeaderFor(player);
}

void GameInstance::onPlayerJoined(ConnectedPlayer& player) {
  flow_.onPlayerJoined(player);
}

std::vector<ConnectedPlayer> GameInstance::takeDepartedPlayers() {
  return sessions_.takeDepartedPlayers();
}

void GameInstance::onTick(
    std::span<const network::IncomingDatagram> datagrams) {
  sessions_.readDatagrams(datagrams);
  sessions_.closeSilentSessions();
  flow_.onTick();
  sessions_.endTick();
}

void GameInstance::finish() { flow_.finish(); }

void GameInstance::removeShipOf(const ConnectedPlayer& departed) {
  if (flow_.world().removePlayer(departed.shipEntityId)) {
    events_.publish(
        game::EntityDestruction{.networkId = departed.shipEntityId,
                                .cause = game::DestructionCause::Removed});
  }
}

}  // namespace rtype::server
