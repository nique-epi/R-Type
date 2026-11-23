#include "RtypeRulesModule.hpp"
#include <memory>
#include <string>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "EntityDestruction.hpp"
#include "GameContext.hpp"
#include "GameFlow.hpp"
#include "GameFlowParts.hpp"
#include "GameInstanceConstants.hpp"
#include "IGameFlow.hpp"
#include "IGameReplication.hpp"
#include "IGameWorld.hpp"
#include "IThreatLevel.hpp"
#include "InstallContext.hpp"
#include "PlayerController.hpp"
#include "PlayerInput.hpp"
#include "RtypeSystems.hpp"
#include "ScoreKeeper.hpp"
#include "SessionHooks.hpp"
#include "ShipInput.hpp"
#include "TimeConstants.hpp"
#include "World.hpp"

namespace rtype::server {

std::string RtypeRulesModule::name() const {
  return std::string{RTYPE_RULES_MODULE_NAME};
}

void RtypeRulesModule::install(GameContext& game,
                               engine::InstallContext& install) {
  game_ = &game;
  replication_ = &install.require<IGameReplication>();
  threat_ = install.find<IThreatLevel>();
  flow_ = std::make_unique<GameFlow>(GameFlowParts{.game = game,
                                                   .world = world_,
                                                   .replication = *replication_,
                                                   .threat = threat_});
  install.provide<IGameFlow>(*flow_);
  install.provide<IGameWorld>(world_);
  install.addWork(
      GAME_SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { flow_->onTick(); });
}

void RtypeRulesModule::start() {
  addRtypeSystems(*game_, threat_);
  auto scoreKeeper =
      std::make_unique<ScoreKeeper>(game_->events, game_->roster);
  flow_->begin();
  scoreKeeper_ = std::move(scoreKeeper);
  addSessionHooks();
}

void RtypeRulesModule::stop() {
  flow_.reset();
  scoreKeeper_.reset();
}

game::World& RtypeRulesModule::world() { return world_.current(); }

void RtypeRulesModule::addSessionHooks() {
  SessionHooks& hooks = game_->hooks;
  hooks.joined.emplace_back([this](ConnectedPlayer& player) {
    player.livesWithoutShip = game_->settings.rules.lives;
    replication_->onPlayerJoined(player);
  });
  hooks.closing.emplace_back(
      [this](const ConnectedPlayer& departed) { removeShipOf(departed); });
  hooks.newerInput.emplace_back(
      [this](const ConnectedPlayer& player, const network::PlayerInput& input) {
        noteTaps(world(), player, input);
        steerShipByInput(world(), player, input);
      });
}

void RtypeRulesModule::removeShipOf(const ConnectedPlayer& departed) {
  if (world().removePlayer(departed.shipEntityId)) {
    game_->events.get().publish(
        game::EntityDestruction{.networkId = departed.shipEntityId,
                                .cause = game::DestructionCause::Removed});
  }
}

}  // namespace rtype::server
