#include "GameFlow.hpp"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "CollisionRules.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "DamageRules.hpp"
#include "EntityIdConstants.hpp"
#include "EventBus.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "GameOutcome.hpp"
#include "GameState.hpp"
#include "GameStateChanged.hpp"
#include "IGameReplication.hpp"
#include "LifeKeeper.hpp"
#include "LifeKeeperParts.hpp"
#include "LivesConstants.hpp"
#include "Logger.hpp"
#include "PlayerController.hpp"
#include "PlayerLives.hpp"
#include "PlayerRoster.hpp"
#include "ShooterViews.hpp"
#include "TimeConstants.hpp"
#include "WaveSpawner.hpp"
#include "WaveSpawnerParts.hpp"
#include "WeaponSystem.hpp"
#include "WeaponSystemParts.hpp"
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

/** @brief Why a game finished, and how it ended. */
struct GameEnding {
  std::string_view reason;
  GameOutcome outcome{GameOutcome::Defeat};
};

/**
 * @returns Why a game played by @p roster in @p world is lost: no player is
 * left in it, or no player left has a life. Nothing otherwise.
 */
std::optional<std::string_view> reasonToLose(PlayerRoster& roster,
                                             game::World& world) {
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

/**
 * @returns How the game in progress played by @p roster in @p world, whose
 * Bydos come from @p spawner, ends now: lost when no player is left or none
 * of them has a life, otherwise won when the last wave of the level is over.
 * The loss comes first, so a game lost on the tick its last wave ends is
 * lost. Nothing when it goes on.
 */
std::optional<GameEnding> endingOf(PlayerRoster& roster, game::World& world,
                                   const WaveSpawner& spawner) {
  const std::optional<std::string_view> loss = reasonToLose(roster, world);
  if (loss.has_value()) {
    return GameEnding{.reason = *loss, .outcome = GameOutcome::Defeat};
  }
  if (spawner.lastWaveFinished()) {
    return GameEnding{.reason = LAST_WAVE_OVER_FINISH_REASON,
                      .outcome = GameOutcome::Victory};
  }
  return std::nullopt;
}

/**
 * @brief Makes the ship of every player who is away want nothing (see
 * silenceAwayShips), gives every missile of a player the view of its player at
 * tick @p tick (see giveShooterViews), then runs the systems of @p world for
 * one tick.
 */
void runWorldSystems(GameContext& game, game::World& world,
                     std::uint32_t tick) {
  silenceAwayShips(world, game.roster.get());
  giveShooterViews(world, game.roster.get(), tick);
  game.systems.get().run(world.components(), engine::SIMULATION_TICK_DURATION);
}

/** @brief Moves @p state to @p next and tells @p events so, with the state it
 * left. */
void changeState(GameState& state, GameState next, engine::EventBus& events) {
  events.publish(GameStateChanged{.previous = std::exchange(state, next),
                                  .current = next});
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
        changeState(state_, GameState::Starting, game_.events.get());
        startBarrier_.open();
      } else {
        replication_.onWaitingTick();
      }
      break;
    case GameState::Starting:
      if (startBarrier_.advance()) {
        changeState(state_, GameState::InProgress, game_.events.get());
        logger_.info("game started, players connected: ",
                     game_.roster.get().connected().size());
      }
      break;
    case GameState::InProgress:
      ++tick_;
      runWorldSystems(game_, world(), tick_);
      weaponSystem_->onTick();
      waveSpawner_->onTick();
      lifeKeeper_->onTick();
      break;
    case GameState::Finished:
      ++finishedTicks_;
      if (finishedTicks_ >= FINISHED_STATE_TICKS) {
        startNewWorld();
        changeState(state_, GameState::WaitingForPlayers, game_.events.get());
      } else {
        replication_.onFinishedTick();
      }
      break;
  }
}

void GameFlow::finish() {
  if (state_ == GameState::InProgress) {
    changeState(state_, GameState::Finished, game_.events.get());
    finishedTicks_ = 0;
  }
}

void GameFlow::finishIfOver() {
  if (state_ != GameState::InProgress) {
    return;
  }
  const std::optional<GameEnding> ending =
      endingOf(game_.roster.get(), world(), *waveSpawner_);
  if (!ending.has_value()) {
    return;
  }
  logger_.info("game finished: ", ending->reason);
  finish();
  replication_.onGameFinished(ending->outcome);
}

void GameFlow::buildWorld() {
  game::World& world = worldSlot_.renew();
  replication_.onWorldBuilt(world);
  collisionRules_ = std::make_unique<game::CollisionRules>(game_.events.get(),
                                                           world.components());
  damageRules_ = std::make_unique<game::DamageRules>(game_.events.get(),
                                                     world.components());
  waveSpawner_ = std::make_unique<WaveSpawner>(
      WaveSpawnerParts{.world = world,
                       .roster = game_.roster.get(),
                       .send = game_.send,
                       .seed = game_.settings.seed + worldsBuilt_,
                       .waves = game_.settings.waves,
                       .types = game_.settings.types});
  weaponSystem_ = std::make_unique<WeaponSystem>(
      WeaponSystemParts{.world = world,
                        .roster = game_.roster.get(),
                        .send = game_.send,
                        .types = game_.settings.types,
                        .events = game_.events.get()});
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
