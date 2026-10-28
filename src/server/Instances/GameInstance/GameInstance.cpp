#include "GameInstance.hpp"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "BydoSpawnerParts.hpp"
#include "CollisionRules.hpp"
#include "CollisionSystem.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayerConstants.hpp"
#include "ConnectedPlayers.hpp"
#include "DamageRules.hpp"
#include "Endpoint.hpp"
#include "EntityDestruction.hpp"
#include "EntityIdConstants.hpp"
#include "EventBus.hpp"
#include "GameEndAnnouncer.hpp"
#include "GameInstanceConstants.hpp"
#include "GameSessionsParts.hpp"
#include "GameSettings.hpp"
#include "GameStarted.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "LifeKeeper.hpp"
#include "LifeKeeperParts.hpp"
#include "MovementSystem.hpp"
#include "PacketHeader.hpp"
#include "PatternSystem.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "PlayfieldExitSystem.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "SessionDatagram.hpp"
#include "ShipControl.hpp"
#include "ShipMotion.hpp"
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
 * @brief Gives the ship of @p player in @p world the presses of @p input,
 * which may start a roll or a dash (game::motionAfterInput()); nothing without
 * a world or a ship. Runs before @p input becomes the newest input of the
 * player, which is what it compares to.
 */
void noteTaps(game::World* world, const ConnectedPlayer& player,
              const network::PlayerInput& input) {
  game::ShipMotion* const motion =
      world == nullptr ? nullptr : world->shipMotionOf(player.shipEntityId);
  if (motion == nullptr) {
    return;
  }
  const game::ShipDirections before = player.newestInput.has_value()
                                          ? directionsOf(*player.newestInput)
                                          : game::ShipDirections{};
  *motion = game::motionAfterInput(
      *motion, game::HeldChange{.before = before,
                                .now = directionsOf(input),
                                .inputNumber = input.inputNumber});
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

}  // namespace

GameInstance::GameInstance(Send send, GameSettings settings,
                           engine::EventBus& events)
    : send_(std::move(send)),
      events_(events),
      sessions_(GameSessionsParts{
          .events = events,
          .send = send_,
          .readyAccepted =
              [this] { return state_ == GameState::WaitingForPlayers; },
          .onClosing =
              [this](const ConnectedPlayer& departed) {
                removeShipOf(departed);
              },
          .onNewerInput =
              [this](const ConnectedPlayer& player,
                     const network::PlayerInput& input) {
                noteTaps(world_.get(), player, input);
              }}),
      scoreKeeper_(events, sessions_.roster()),
      worldStateSender_(sessions_.roster(), send_),
      seed_(settings.seed),
      wavePlan_(std::move(settings.waves)),
      logger_(std::string{INSTANCES_LOGGER_NAME}) {
  systems_.add(std::make_unique<game::PatternSystem>());
  systems_.add(std::make_unique<game::MovementSystem>());
  systems_.add(std::make_unique<game::PlayfieldExitSystem>(events));
  systems_.add(std::make_unique<game::CollisionSystem>(events));
  startNewWorld();
}

GameState GameInstance::state() const { return state_; }

std::uint32_t GameInstance::tick() const { return tick_; }

PlayerRoster& GameInstance::roster() { return sessions_.roster(); }

game::World& GameInstance::world() { return *world_; }

network::PacketHeader GameInstance::nextSessionHeaderFor(
    const network::Endpoint& player) {
  return sessions_.nextSessionHeaderFor(player);
}

void GameInstance::onPlayerJoined(ConnectedPlayer& player) {
  shipSpawnAnnouncer_->onPlayerJoined(player);
}

std::vector<ConnectedPlayer> GameInstance::takeDepartedPlayers() {
  return sessions_.takeDepartedPlayers();
}

void GameInstance::onTick(
    std::span<const network::IncomingDatagram> datagrams) {
  sessions_.readDatagrams(datagrams);
  sessions_.closeSilentSessions();
  finishIfOver();
  advanceState();
  if (state_ == GameState::InProgress) {
    worldStateSender_.onTick(tick_, world_->components());
  }
  sessions_.endTick();
}

void GameInstance::advanceState() {
  switch (state_) {
    case GameState::WaitingForPlayers:
      if (sessions_.everyPlayerIsReady()) {
        startGame();
      } else {
        lobbyStateAnnouncer_->onWaitingTick();
      }
      break;
    case GameState::InProgress:
      ++tick_;
      moveShips();
      systems_.run(world_->components(), engine::SIMULATION_TICK_DURATION);
      missileLauncher_->onTick(shootersIn(sessions_.roster()));
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

void GameInstance::finish() {
  if (state_ == GameState::InProgress) {
    state_ = GameState::Finished;
    finishedTicks_ = 0;
  }
}

void GameInstance::removeShipOf(const ConnectedPlayer& departed) {
  if (world_->removePlayer(departed.shipEntityId)) {
    events_.publish(
        game::EntityDestruction{.networkId = departed.shipEntityId,
                                .cause = game::DestructionCause::Removed});
  }
}

void GameInstance::finishIfOver() {
  const std::optional<std::string_view> reason =
      reasonToFinish(state_, sessions_.roster());
  if (!reason.has_value()) {
    return;
  }
  logger_.info("game finished: ", *reason);
  finish();
  gameEndAnnouncer_ =
      std::make_unique<GameEndAnnouncer>(sessions_.roster(), send_);
}

void GameInstance::startGame() {
  state_ = GameState::InProgress;
  const ConnectedPlayers players = sessions_.roster().connected();
  for (ConnectedPlayer* const player : players) {
    send_(sessionDatagram(*player, network::GameStarted{}));
  }
  logger_.info("game started, players connected: ", players.size());
}

void GameInstance::moveShips() {
  for (const ConnectedPlayer* const player : sessions_.roster().connected()) {
    const std::optional<ShipMove> move = shipMoveOf(*world_, *player);
    if (move.has_value()) {
      moveOneTick(*move);
    }
  }
}

void GameInstance::buildWorld() {
  world_ = std::make_unique<game::World>();
  collisionRules_ =
      std::make_unique<game::CollisionRules>(events_, world_->components());
  damageRules_ =
      std::make_unique<game::DamageRules>(events_, world_->components());
  shipSpawnAnnouncer_ =
      std::make_unique<ShipSpawnAnnouncer>(*world_, sessions_.roster(), send_);
  bydoSpawner_ = std::make_unique<BydoSpawner>(
      BydoSpawnerParts{.world = *world_,
                       .roster = sessions_.roster(),
                       .send = send_,
                       .seed = seed_ + worldsBuilt_,
                       .waves = wavePlan_});
  missileLauncher_ =
      std::make_unique<MissileLauncher>(*world_, sessions_.roster(), send_);
  bydoGunner_ =
      std::make_unique<BydoGunner>(*world_, sessions_.roster(), send_);
  lifeKeeper_ = std::make_unique<LifeKeeper>(
      LifeKeeperParts{.events = events_,
                      .world = *world_,
                      .roster = sessions_.roster(),
                      .respawn = [this](ConnectedPlayer& player) {
                        shipSpawnAnnouncer_->onPlayerJoined(player);
                      }});
  lobbyStateAnnouncer_ =
      std::make_unique<LobbyStateAnnouncer>(sessions_.roster(), send_);
  gameEndAnnouncer_.reset();
}

void GameInstance::startNewWorld() {
  buildWorld();
  ++worldsBuilt_;
  tick_ = 0;
  finishedTicks_ = 0;
  for (ConnectedPlayer* const player : sessions_.roster().connected()) {
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
