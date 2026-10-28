#include "GameInstance.hpp"
#include <cstdint>
#include <memory>
#include <span>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "GameInstanceConstants.hpp"
#include "GameInstanceParts.hpp"
#include "GameModules.hpp"
#include "GameSessionsParts.hpp"
#include "GameState.hpp"
#include "IGameFlow.hpp"
#include "IGameModule.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "TimeConstants.hpp"
#include "World.hpp"

namespace rtype::server {

GameInstance::GameInstance(GameInstanceParts parts,
                           std::vector<std::unique_ptr<IGameModule>> modules)
    : settings_(std::move(parts.settings)),
      sessions_(GameSessionsParts{
          .events = parts.events,
          .send = parts.send,
          .readyAccepted =
              [this] { return flow_->state() == GameState::WaitingForPlayers; },
          .onClosing =
              [this](const ConnectedPlayer& departed) {
                flow_->onPlayerLeaving(departed);
              },
          .onNewerInput =
              [this](const ConnectedPlayer& player,
                     const network::PlayerInput& input) {
                flow_->onNewerInput(player, input);
              }}),
      context_{.events = parts.events,
               .timers = parts.timers,
               .systems = systems_,
               .world = world_,
               .roster = sessions_.roster(),
               .settings = settings_,
               .send = std::move(parts.send),
               .everyPlayerIsReady =
                   [this] { return sessions_.everyPlayerIsReady(); }},
      modules_(std::move(modules)) {
  modules_.addWork(GAME_INPUT_STAGE,
                   [this]([[maybe_unused]] engine::Duration elapsed) {
                     sessions_.readDatagrams(datagrams_);
                     sessions_.closeSilentSessions();
                   });
  modules_.addWork(GAME_OUTPUT_STAGE,
                   [this]([[maybe_unused]] engine::Duration elapsed) {
                     sessions_.endTick();
                   });
  modules_.installAndStart(context_);
  flow_ = &modules_.require<IGameFlow>();
}

GameState GameInstance::state() const { return flow_->state(); }

std::uint32_t GameInstance::tick() const { return flow_->tick(); }

PlayerRoster& GameInstance::roster() { return sessions_.roster(); }

game::World& GameInstance::world() { return world_.current(); }

network::PacketHeader GameInstance::nextSessionHeaderFor(
    const network::Endpoint& player) {
  return sessions_.nextSessionHeaderFor(player);
}

void GameInstance::onPlayerJoined(ConnectedPlayer& player) {
  flow_->onPlayerJoined(player);
}

std::vector<ConnectedPlayer> GameInstance::takeDepartedPlayers() {
  return sessions_.takeDepartedPlayers();
}

void GameInstance::onTick(
    std::span<const network::IncomingDatagram> datagrams) {
  datagrams_ = datagrams;
  modules_.runStages(engine::SIMULATION_TICK_DURATION);
  datagrams_ = {};
}

void GameInstance::finish() { flow_->finish(); }

}  // namespace rtype::server
