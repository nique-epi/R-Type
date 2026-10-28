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
#include "SessionHooks.hpp"
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
                    .seed = game_->settings.get().seed,
                    .waves = game_->settings.get().waves});
  scoreKeeper_ = std::move(scoreKeeper);
  flow_ = std::move(flow);
  addSessionHooks();
}

void RtypeRulesModule::stop() {
  flow_.reset();
  scoreKeeper_.reset();
}

GameState RtypeRulesModule::state() const { return flow_->state(); }

std::uint32_t RtypeRulesModule::tick() const { return flow_->tick(); }

void RtypeRulesModule::finish() { flow_->finish(); }

void RtypeRulesModule::addSessionHooks() {
  SessionHooks& hooks = game_->hooks;
  hooks.joined.emplace_back([this](ConnectedPlayer& player) {
    replication_->onPlayerJoined(player);
  });
  hooks.closing.emplace_back(
      [this](const ConnectedPlayer& departed) { removeShipOf(departed); });
  hooks.newerInput.emplace_back(
      [this](const ConnectedPlayer& player, const network::PlayerInput& input) {
        noteTaps(flow_->world(), player, input);
      });
}

void RtypeRulesModule::removeShipOf(const ConnectedPlayer& departed) {
  if (flow_->world().removePlayer(departed.shipEntityId)) {
    game_->events.get().publish(
        game::EntityDestruction{.networkId = departed.shipEntityId,
                                .cause = game::DestructionCause::Removed});
  }
}

}  // namespace rtype::server
