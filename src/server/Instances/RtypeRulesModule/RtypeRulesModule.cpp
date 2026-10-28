#include "RtypeRulesModule.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "EntityDestruction.hpp"
#include "GameContext.hpp"
#include "GameFlow.hpp"
#include "GameFlowParts.hpp"
#include "GameInstanceConstants.hpp"
#include "GameState.hpp"
#include "IGameFlow.hpp"
#include "IGameReplication.hpp"
#include "InstallContext.hpp"
#include "PlayerInput.hpp"
#include "RtypeSystems.hpp"
#include "ScoreKeeper.hpp"
#include "ShipInput.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

std::string RtypeRulesModule::name() const {
  return std::string{RTYPE_RULES_MODULE_NAME};
}

void RtypeRulesModule::install(GameContext& game,
                               engine::InstallContext& install) {
  game_ = &game;
  replication_ = &install.require<IGameReplication>();
  install.provide<IGameFlow>(*this);
  install.addWork(
      GAME_SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { flow_->onTick(); });
}

void RtypeRulesModule::start() {
  addRtypeSystems(game_->systems, game_->events);
  auto scoreKeeper =
      std::make_unique<ScoreKeeper>(game_->events, game_->roster);
  auto flow = std::make_unique<GameFlow>(
      GameFlowParts{.events = game_->events,
                    .systems = game_->systems,
                    .world = game_->world,
                    .roster = game_->roster,
                    .replication = std::ref(*replication_),
                    .send = game_->send,
                    .everyPlayerIsReady = game_->everyPlayerIsReady,
                    .settings = game_->settings});
  scoreKeeper_ = std::move(scoreKeeper);
  flow_ = std::move(flow);
}

void RtypeRulesModule::stop() {
  flow_.reset();
  scoreKeeper_.reset();
}

GameState RtypeRulesModule::state() const { return flow_->state(); }

std::uint32_t RtypeRulesModule::tick() const { return flow_->tick(); }

void RtypeRulesModule::onPlayerJoined(ConnectedPlayer& player) {
  flow_->onPlayerJoined(player);
}

void RtypeRulesModule::finish() { flow_->finish(); }

void RtypeRulesModule::onPlayerLeaving(const ConnectedPlayer& player) {
  if (flow_->world().removePlayer(player.shipEntityId)) {
    game_->events.get().publish(
        game::EntityDestruction{.networkId = player.shipEntityId,
                                .cause = game::DestructionCause::Removed});
  }
}

void RtypeRulesModule::onNewerInput(const ConnectedPlayer& player,
                                    const network::PlayerInput& input) {
  noteTaps(flow_->world(), player, input);
}

}  // namespace rtype::server
