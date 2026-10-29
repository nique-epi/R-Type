#include "GameFlow.hpp"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include "BydoSpawnerParts.hpp"
#include "CollisionRules.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayerConstants.hpp"
#include "ConnectedPlayers.hpp"
#include "DamageRules.hpp"
#include "EntityIdConstants.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "GameStarted.hpp"
#include "GameState.hpp"
#include "IGameReplication.hpp"
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
#include "WorldSlot.hpp"

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

GameFlow::GameFlow(GameContext& game, WorldSlot& world,
                   IGameReplication& replication)
    : game_(game),
      worldSlot_(world),
      replication_(replication),
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
        state_ = GameState::InProgress;
        announceGameStart(game_.roster.get(), game_.send, logger_);
      } else {
        replication_.onWaitingTick();
      }
      break;
    case GameState::InProgress: {
      game::World& currentWorld = world();
      ++tick_;
      moveShips(currentWorld, game_.roster.get());
      game_.systems.get().run(currentWorld.components(),
                              engine::SIMULATION_TICK_DURATION);
      missileLauncher_->onTick(shootersIn(game_.roster.get()));
      bydoGunner_->onTick();
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
      reasonToFinish(state_, game_.roster.get());
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
  missileLauncher_ =
      std::make_unique<MissileLauncher>(world, game_.roster.get(), game_.send);
  bydoGunner_ =
      std::make_unique<BydoGunner>(world, game_.roster.get(), game_.send);
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
    player->lives = STARTING_LIVES;
    player->ticksUntilRespawn = 0;
    player->invulnerableTicksLeft = 0;
    player->shipEntityId = identifiers::NO_ENTITY_ID;
    replication_.onPlayerJoined(*player);
  }
}

}  // namespace rtype::server
