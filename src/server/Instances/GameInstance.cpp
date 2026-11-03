#include "GameInstance.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DelayedInputs.hpp"
#include "Endpoint.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "GameInstanceParts.hpp"
#include "GameModules.hpp"
#include "GameSessions.hpp"
#include "GameSessionsParts.hpp"
#include "GameState.hpp"
#include "IGameFlow.hpp"
#include "IGameModule.hpp"
#include "IGameWorld.hpp"
#include "IncomingDatagram.hpp"
#include "LeaderboardRequest.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "SessionHooks.hpp"
#include "SystemScheduler.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

template <typename Hooks, typename... Arguments>
void callAll(const Hooks& hooks, Arguments&... arguments) {
  for (const auto& hook : hooks) {
    hook(arguments...);
  }
}

/** @brief What the game owns, for its sessions to call and its modules to work
 * on. */
struct GameOwnParts {
  std::reference_wrapper<DelayedInputs> delayedInputs;
  std::reference_wrapper<engine::SystemScheduler> systems;
  std::reference_wrapper<SessionHooks> hooks;
};

/** @returns What the sessions of a game are built from. */
GameSessionsParts sessionPartsOf(const GameInstanceParts& parts,
                                 GameOwnParts own,
                                 std::function<bool()> readyAccepted) {
  return GameSessionsParts{
      .events = parts.events,
      .send = parts.send,
      .clock = parts.clock,
      .readyAccepted = std::move(readyAccepted),
      .onClosing =
          [own](const ConnectedPlayer& departed) {
            own.delayedInputs.get().clear(departed.playerNumber);
            callAll(own.hooks.get().closing, departed);
          },
      .onNewerInput =
          [own](const ConnectedPlayer& player,
                const network::PlayerInput& input) {
            own.delayedInputs.get().push(player.playerNumber, input);
          },
      .onLeaderboardRequest =
          [own](const ConnectedPlayer& player,
                const network::LeaderboardRequest& request) {
            callAll(own.hooks.get().leaderboardRequested, player, request);
          }};
}

/** @returns What a game gives the modules it installs. */
GameContext contextOf(GameInstanceParts parts, GameSessions& sessions,
                      GameOwnParts own) {
  return GameContext{.events = parts.events,
                     .timers = parts.timers,
                     .systems = own.systems,
                     .roster = sessions.roster(),
                     .hooks = own.hooks,
                     .settings = std::move(parts.settings),
                     .send = std::move(parts.send),
                     .everyPlayerIsReady = [&sessions] {
                       return sessions.everyPlayerIsReady();
                     }};
}

}  // namespace

GameInstance::GameInstance(GameInstanceParts parts,
                           std::vector<std::unique_ptr<IGameModule>> modules)
    : delayedInputs_(parts.settings.inputDelayTicks),
      sessions_(sessionPartsOf(
          parts,
          GameOwnParts{.delayedInputs = delayedInputs_,
                       .systems = systems_,
                       .hooks = hooks_},
          [this] { return flow_->state() == GameState::WaitingForPlayers; })),
      context_(contextOf(std::move(parts), sessions_,
                         GameOwnParts{.delayedInputs = delayedInputs_,
                                      .systems = systems_,
                                      .hooks = hooks_})),
      modules_(std::move(modules)) {
  modules_.addWork(
      GAME_INPUT_STAGE, [this]([[maybe_unused]] engine::Duration elapsed) {
        sessions_.startTick();
        sessions_.readDatagrams(datagrams_);
        delayedInputs_.release(sessions_.roster(),
                               [this](const ConnectedPlayer& player,
                                      const network::PlayerInput& input) {
                                 callAll(hooks_.newerInput, player, input);
                               });
        sessions_.closeSilentSessions();
      });
  modules_.addWork(GAME_OUTPUT_STAGE,
                   [this]([[maybe_unused]] engine::Duration elapsed) {
                     sessions_.endTick();
                   });
  modules_.installAndStart(context_);
  flow_ = &modules_.require<IGameFlow>();
  world_ = &modules_.require<IGameWorld>();
}

GameState GameInstance::state() const { return flow_->state(); }

std::uint32_t GameInstance::tick() const { return flow_->tick(); }

std::size_t GameInstance::entityCount() const { return world_->entityCount(); }

engine::SystemScheduler& GameInstance::systems() { return systems_; }

PlayerRoster& GameInstance::roster() { return sessions_.roster(); }

network::PacketHeader GameInstance::nextSessionHeaderFor(
    const network::Endpoint& player) {
  return sessions_.nextSessionHeaderFor(player);
}

void GameInstance::onPlayerJoined(ConnectedPlayer& player) const {
  callAll(hooks_.joined, player);
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
