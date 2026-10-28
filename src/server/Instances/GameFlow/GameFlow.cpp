#include "GameFlow.hpp"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "BydoSpawnerParts.hpp"
#include "CollisionRules.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayerConstants.hpp"
#include "ConnectedPlayers.hpp"
#include "DamageRules.hpp"
#include "EntityIdConstants.hpp"
#include "GameFlowParts.hpp"
#include "GameInstanceConstants.hpp"
#include "GameStarted.hpp"
#include "GameState.hpp"
#include "LifeKeeper.hpp"
#include "LifeKeeperParts.hpp"
#include "Logger.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "SessionDatagram.hpp"
#include "ShipControl.hpp"
#include "ShipInput.hpp"
#include "ShipMotion.hpp"
#include "TimeConstants.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The connected players of @p roster whose newest input holds shoot.
 */
ConnectedPlayers shootersIn(PlayerRoster& roster) {
  ConnectedPlayers shooters;
  for (ConnectedPlayer* const player : roster.connected()) {
    if (player->newestInput.has_value() &&
        holdsActionAt(*player->newestInput,
                      network::SHOOT_ACTION_BIT_POSITION)) {
      shooters.append(player);
    }
  }
  return shooters;
}

/** @brief A ship to move this tick, how it moves, and the directions its
 * player holds. */
struct ShipMove {
  game::Position* ship{nullptr};
  game::ShipMotion* motion{nullptr};
  game::ShipDirections held;
};

/**
 * @returns The ship of @p player in @p world, its motion and the directions
 * its newest input holds; nothing when the player has no ship in that world or
 * sent no input yet.
 */
std::optional<ShipMove> shipMoveOf(game::World& world,
                                   const ConnectedPlayer& player) {
  game::Position* const ship = world.shipPositionOf(player.shipEntityId);
  game::ShipMotion* const motion = world.shipMotionOf(player.shipEntityId);
  if (ship == nullptr || motion == nullptr || !player.newestInput.has_value()) {
    return std::nullopt;
  }
  return ShipMove{.ship = ship,
                  .motion = motion,
                  .held = directionsOf(*player.newestInput)};
}

/** @brief Moves the ship of @p move by one tick of the directions it holds. */
void moveOneTick(const ShipMove& move) {
  const game::MovingShip moved = game::shipAfter(
      game::MovingShip{.position = *move.ship, .motion = *move.motion},
      move.held, engine::SIMULATION_TICK_DURATION);
  *move.ship = moved.position;
  *move.motion = moved.motion;
}

/**
 * @brief Moves the ship of every player of @p roster who sent an input, in
 * @p world, by one tick of what that input holds.
 */
void moveShips(game::World& world, PlayerRoster& roster) {
  for (const ConnectedPlayer* const player : roster.connected()) {
    const std::optional<ShipMove> move = shipMoveOf(world, *player);
    if (move.has_value()) {
      moveOneTick(*move);
    }
  }
}

/**
 * @returns Whether @p player has a life left, the one of a ship it waits for
 * included.
 */
bool hasALifeLeft(const ConnectedPlayer* player) { return player->lives > 0; }

/**
 * @returns Why a game in @p state, played by @p roster, must finish now: it is
 * in progress with no player left in it, or with no player left who has a
 * life. Nothing otherwise.
 */
std::optional<std::string_view> reasonToFinish(GameState state,
                                               PlayerRoster& roster) {
  if (state != GameState::InProgress) {
    return std::nullopt;
  }
  const ConnectedPlayers players = roster.connected();
  if (players.empty()) {
    return NO_PLAYER_LEFT_FINISH_REASON;
  }
  if (std::ranges::none_of(players, hasALifeLeft)) {
    return NO_LIFE_LEFT_FINISH_REASON;
  }
  return std::nullopt;
}

/**
 * @brief Sends every connected player of @p roster GameStarted through
 * @p send, then logs that the game started.
 */
void announceGameStart(
    PlayerRoster& roster,
    const std::function<void(network::OutgoingDatagram)>& send,
    const logging::Logger& logger) {
  const ConnectedPlayers players = roster.connected();
  for (ConnectedPlayer* const player : players) {
    send(sessionDatagram(*player, network::GameStarted{}));
  }
  logger.info("game started, players connected: ", players.size());
}

}  // namespace

GameFlow::GameFlow(GameFlowParts parts)
    : events_(parts.events),
      systems_(parts.systems),
      worldSlot_(parts.world),
      roster_(parts.roster),
      replication_(parts.replication),
      send_(std::move(parts.send)),
      everyPlayerIsReady_(std::move(parts.everyPlayerIsReady)),
      seed_(parts.seed),
      wavePlan_(parts.waves),
      logger_(std::string{INSTANCES_LOGGER_NAME}) {
  startNewWorld();
}

GameState GameFlow::state() const { return state_; }

std::uint32_t GameFlow::tick() const { return tick_; }

game::World& GameFlow::world() { return worldSlot_.get().current(); }

void GameFlow::onPlayerJoined(ConnectedPlayer& player) {
  replication_.get().onPlayerJoined(player);
}

void GameFlow::onTick() {
  finishIfOver();
  advanceState();
  if (state_ == GameState::InProgress) {
    replication_.get().onProgressTick(tick_, world().components());
  }
}

void GameFlow::advanceState() {
  switch (state_) {
    case GameState::WaitingForPlayers:
      if (everyPlayerIsReady_()) {
        state_ = GameState::InProgress;
        announceGameStart(roster_.get(), send_, logger_);
      } else {
        replication_.get().onWaitingTick();
      }
      break;
    case GameState::InProgress:
      ++tick_;
      moveShips(world(), roster_.get());
      systems_.get().run(world().components(),
                         engine::SIMULATION_TICK_DURATION);
      missileLauncher_->onTick(shootersIn(roster_.get()));
      bydoGunner_->onTick();
      bydoSpawner_->onTick();
      lifeKeeper_->onTick();
      break;
    case GameState::Finished:
      ++finishedTicks_;
      if (finishedTicks_ >= FINISHED_STATE_TICKS) {
        startNewWorld();
        state_ = GameState::WaitingForPlayers;
      } else {
        replication_.get().onFinishedTick();
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
      reasonToFinish(state_, roster_.get());
  if (!reason.has_value()) {
    return;
  }
  logger_.info("game finished: ", *reason);
  finish();
  replication_.get().onGameFinished();
}

void GameFlow::buildWorld() {
  game::World& world = worldSlot_.get().renew();
  replication_.get().onWorldBuilt(world);
  collisionRules_ =
      std::make_unique<game::CollisionRules>(events_.get(), world.components());
  damageRules_ =
      std::make_unique<game::DamageRules>(events_.get(), world.components());
  bydoSpawner_ = std::make_unique<BydoSpawner>(
      BydoSpawnerParts{.world = world,
                       .roster = roster_.get(),
                       .send = send_,
                       .seed = seed_ + worldsBuilt_,
                       .waves = wavePlan_.get()});
  missileLauncher_ =
      std::make_unique<MissileLauncher>(world, roster_.get(), send_);
  bydoGunner_ = std::make_unique<BydoGunner>(world, roster_.get(), send_);
  lifeKeeper_ = std::make_unique<LifeKeeper>(
      LifeKeeperParts{.events = events_.get(),
                      .world = world,
                      .roster = roster_.get(),
                      .respawn = [this](ConnectedPlayer& player) {
                        replication_.get().onPlayerJoined(player);
                      }});
}

void GameFlow::startNewWorld() {
  buildWorld();
  ++worldsBuilt_;
  tick_ = 0;
  finishedTicks_ = 0;
  for (ConnectedPlayer* const player : roster_.get().connected()) {
    player->ready = false;
    player->score = 0;
    player->lives = STARTING_LIVES;
    player->ticksUntilRespawn = 0;
    player->invulnerableTicksLeft = 0;
    player->shipEntityId = identifiers::NO_ENTITY_ID;
    replication_.get().onPlayerJoined(*player);
  }
}

}  // namespace rtype::server
