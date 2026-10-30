#include "GameFlow.hpp"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include "BydoSpawnerParts.hpp"
#include "CollisionRules.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "DamageRules.hpp"
#include "EntityIdConstants.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "GameState.hpp"
#include "IGameReplication.hpp"
#include "LifeKeeper.hpp"
#include "LifeKeeperParts.hpp"
#include "LivesConstants.hpp"
#include "Logger.hpp"
#include "PlayerController.hpp"
#include "PlayerLives.hpp"
#include "PlayerRoster.hpp"
#include "TimeConstants.hpp"
#include "WeaponSystem.hpp"
#include "World.hpp"
#include "WorldSlot.hpp"

namespace rtype::server {

namespace {

/**
 * @returns Whether @p player has a life left, the one of a ship it waits for
 * included.
 */
bool hasALifeLeft(game::World& world, const ConnectedPlayer* player) {
  return livesOf(world, *player) > 0;
}

/**
 * @returns Why a game in @p state, played by @p roster in @p world, must
 * finish now: it is in progress with no player left in it, or with no player
 * left who has a life. Nothing otherwise.
 */
std::optional<std::string_view> reasonToFinish(GameState state,
                                               PlayerRoster& roster,
                                               game::World& world) {
  if (state != GameState::InProgress) {
    return std::nullopt;
  }
  const ConnectedPlayers players = roster.connected();
  if (players.empty()) {
    return NO_PLAYER_LEFT_FINISH_REASON;
  }
  if (std::ranges::none_of(players, [&world](const ConnectedPlayer* player) {
        return hasALifeLeft(world, player);
      })) {
    return NO_LIFE_LEFT_FINISH_REASON;
  }
  return std::nullopt;
}

}  // namespace

GameFlow::GameFlow(GameContext& game, WorldSlot& world,
                   IGameReplication& replication)
    : game_(game),
      worldSlot_(world),
      replication_(replication),
      startBarrier_(game.roster.get(), game.send),
      logger_(std::string{INSTANCES_LOGGER_NAME}) {}

void GameFlow::begin() { startNewWorld(); }

GameState GameFlow::state() const { return state_; }

std::uint32_t GameFlow::tick() const { return tick_; }

game::World& GameFlow::world() { return worldSlot_.current(); }

void GameFlow::onTick() {
  finishIfOver();
  advanceState();
  if (state_ == GameState::InProgress) {
    replication_.onProgressTick(tick_, world().components());
  }
}

void GameFlow::advanceState() {
  switch (state_) {
    case GameState::WaitingForPlayers:
      if (game_.everyPlayerIsReady()) {
        state_ = GameState::Starting;
        startBarrier_.open();
      } else {
        replication_.onWaitingTick();
      }
      break;
    case GameState::Starting:
      if (startBarrier_.advance()) {
        state_ = GameState::InProgress;
        logger_.info("game started, players connected: ",
                     game_.roster.get().connected().size());
      }
      break;
    case GameState::InProgress: {
      game::World& currentWorld = world();
      ++tick_;
      silenceAwayShips(currentWorld, game_.roster.get());
      game_.systems.get().run(currentWorld.components(),
                              engine::SIMULATION_TICK_DURATION);
      weaponSystem_->onTick();
      bydoSpawner_->onTick();
      lifeKeeper_->onTick();
      break;
    }
    case GameState::Finished:
      ++finishedTicks_;
      if (finishedTicks_ >= FINISHED_STATE_TICKS) {
        startNewWorld();
        state_ = GameState::WaitingForPlayers;
      } else {
        replication_.onFinishedTick();
      }
      break;
  }
}

void GameFlow::finish() {
  if (state_ == GameState::InProgress) {
    state_ = GameState::Finished;
    finishedTicks_ = 0;
  }
}

void GameFlow::finishIfOver() {
  const std::optional<std::string_view> reason =
      reasonToFinish(state_, game_.roster.get(), world());
  if (!reason.has_value()) {
    return;
  }
  logger_.info("game finished: ", *reason);
  finish();
  replication_.onGameFinished();
}

void GameFlow::buildWorld() {
  game::World& world = worldSlot_.renew();
  replication_.onWorldBuilt(world);
  collisionRules_ = std::make_unique<game::CollisionRules>(game_.events.get(),
                                                           world.components());
  damageRules_ = std::make_unique<game::DamageRules>(game_.events.get(),
                                                     world.components());
  bydoSpawner_ = std::make_unique<BydoSpawner>(
      BydoSpawnerParts{.world = world,
                       .roster = game_.roster.get(),
                       .send = game_.send,
                       .seed = game_.settings.seed + worldsBuilt_,
                       .waves = game_.settings.waves});
  weaponSystem_ =
      std::make_unique<WeaponSystem>(world, game_.roster.get(), game_.send);
  lifeKeeper_ = std::make_unique<LifeKeeper>(
      LifeKeeperParts{.events = game_.events.get(),
                      .world = world,
                      .roster = game_.roster.get(),
                      .respawn = [this](ConnectedPlayer& player) {
                        replication_.onPlayerJoined(player);
                      }});
}

void GameFlow::startNewWorld() {
  buildWorld();
  ++worldsBuilt_;
  tick_ = 0;
  finishedTicks_ = 0;
  for (ConnectedPlayer* const player : game_.roster.get().connected()) {
    player->ready = false;
    player->score = 0;
    player->livesWithoutShip = game::STARTING_LIVES;
    player->ticksUntilRespawn = 0;
    player->shipEntityId = identifiers::NO_ENTITY_ID;
    replication_.onPlayerJoined(*player);
  }
}

}  // namespace rtype::server
