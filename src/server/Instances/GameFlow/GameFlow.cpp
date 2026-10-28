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
#include "CollisionSystem.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayerConstants.hpp"
#include "ConnectedPlayers.hpp"
#include "DamageRules.hpp"
#include "EntityIdConstants.hpp"
#include "GameEndAnnouncer.hpp"
#include "GameFlowParts.hpp"
#include "GameInstanceConstants.hpp"
#include "GameStarted.hpp"
#include "GameState.hpp"
#include "LifeKeeper.hpp"
#include "LifeKeeperParts.hpp"
#include "Logger.hpp"
#include "MovementSystem.hpp"
#include "OutgoingDatagram.hpp"
#include "PatternSystem.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "PlayfieldExitSystem.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "SessionDatagram.hpp"
#include "ShipControl.hpp"
#include "TimeConstants.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/**
 * @returns Whether @p input holds the action at @p bitPosition of its actions
 * byte.
 */
bool holdsActionAt(const network::PlayerInput& input,
                   std::uint8_t bitPosition) {
  const unsigned int mask = 1U << static_cast<unsigned int>(bitPosition);
  return (static_cast<unsigned int>(input.actions) & mask) != 0U;
}

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

/**
 * @returns The directions @p input holds for the ship; shooting is not one.
 */
game::ShipDirections directionsOf(const network::PlayerInput& input) {
  return game::ShipDirections{
      .up = holdsActionAt(input, network::UP_ACTION_BIT_POSITION),
      .down = holdsActionAt(input, network::DOWN_ACTION_BIT_POSITION),
      .left = holdsActionAt(input, network::LEFT_ACTION_BIT_POSITION),
      .right = holdsActionAt(input, network::RIGHT_ACTION_BIT_POSITION)};
}

/** @brief A ship to move this tick, and the directions its player holds. */
struct ShipMove {
  game::Position* ship{nullptr};
  game::ShipDirections held;
};

/**
 * @returns The ship of @p player in @p world and the directions its newest
 * input holds; nothing when the player has no ship in that world or sent no
 * input yet.
 */
std::optional<ShipMove> shipMoveOf(game::World& world,
                                   const ConnectedPlayer& player) {
  game::Position* const ship = world.shipPositionOf(player.shipEntityId);
  if (ship == nullptr || !player.newestInput.has_value()) {
    return std::nullopt;
  }
  return ShipMove{.ship = ship, .held = directionsOf(*player.newestInput)};
}

/** @brief Moves the ship of @p move by one tick of the directions it holds. */
void moveOneTick(const ShipMove& move) {
  *move.ship = game::shipPositionAfter(*move.ship, move.held,
                                       engine::SIMULATION_TICK_DURATION);
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
      roster_(parts.roster),
      send_(std::move(parts.send)),
      everyPlayerIsReady_(std::move(parts.everyPlayerIsReady)),
      worldStateSender_(roster_.get(), send_),
      seed_(parts.settings.seed),
      wavePlan_(std::move(parts.settings.waves)),
      logger_(std::string{INSTANCES_LOGGER_NAME}) {
  systems_.add(std::make_unique<game::PatternSystem>());
  systems_.add(std::make_unique<game::MovementSystem>());
  systems_.add(std::make_unique<game::PlayfieldExitSystem>(events_.get()));
  systems_.add(std::make_unique<game::CollisionSystem>(events_.get()));
  startNewWorld();
}

GameState GameFlow::state() const { return state_; }

std::uint32_t GameFlow::tick() const { return tick_; }

game::World& GameFlow::world() { return *world_; }

void GameFlow::onPlayerJoined(ConnectedPlayer& player) {
  shipSpawnAnnouncer_->onPlayerJoined(player);
}

void GameFlow::onTick() {
  finishIfOver();
  advanceState();
  if (state_ == GameState::InProgress) {
    worldStateSender_.onTick(tick_, world_->components());
  }
}

void GameFlow::advanceState() {
  switch (state_) {
    case GameState::WaitingForPlayers:
      if (everyPlayerIsReady_()) {
        state_ = GameState::InProgress;
        announceGameStart(roster_.get(), send_, logger_);
      } else {
        lobbyStateAnnouncer_->onWaitingTick();
      }
      break;
    case GameState::InProgress:
      ++tick_;
      moveShips(*world_, roster_.get());
      systems_.run(world_->components(), engine::SIMULATION_TICK_DURATION);
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
      } else if (gameEndAnnouncer_ != nullptr) {
        gameEndAnnouncer_->onFinishedTick();
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
  gameEndAnnouncer_ = std::make_unique<GameEndAnnouncer>(roster_.get(), send_);
}

void GameFlow::buildWorld() {
  world_ = std::make_unique<game::World>();
  collisionRules_ = std::make_unique<game::CollisionRules>(
      events_.get(), world_->components());
  damageRules_ =
      std::make_unique<game::DamageRules>(events_.get(), world_->components());
  shipSpawnAnnouncer_ =
      std::make_unique<ShipSpawnAnnouncer>(*world_, roster_.get(), send_);
  bydoSpawner_ = std::make_unique<BydoSpawner>(
      BydoSpawnerParts{.world = *world_,
                       .roster = roster_.get(),
                       .send = send_,
                       .seed = seed_ + worldsBuilt_,
                       .waves = wavePlan_});
  missileLauncher_ =
      std::make_unique<MissileLauncher>(*world_, roster_.get(), send_);
  bydoGunner_ = std::make_unique<BydoGunner>(*world_, roster_.get(), send_);
  lifeKeeper_ = std::make_unique<LifeKeeper>(
      LifeKeeperParts{.events = events_.get(),
                      .world = *world_,
                      .roster = roster_.get(),
                      .respawn = [this](ConnectedPlayer& player) {
                        shipSpawnAnnouncer_->onPlayerJoined(player);
                      }});
  lobbyStateAnnouncer_ =
      std::make_unique<LobbyStateAnnouncer>(roster_.get(), send_);
  gameEndAnnouncer_.reset();
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
    shipSpawnAnnouncer_->onPlayerJoined(*player);
  }
}

}  // namespace rtype::server
