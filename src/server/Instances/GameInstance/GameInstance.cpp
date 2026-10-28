#include "GameInstance.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <ratio>
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
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "EntityDestruction.hpp"
#include "EntityIdConstants.hpp"
#include "EventBus.hpp"
#include "GameEndAnnouncer.hpp"
#include "GameInstanceConstants.hpp"
#include "GameSettings.hpp"
#include "GameStarted.hpp"
#include "GameState.hpp"
#include "Heartbeat.hpp"
#include "IncomingDatagram.hpp"
#include "LifeKeeper.hpp"
#include "LifeKeeperParts.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MovementSystem.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "PatternSystem.hpp"
#include "PlayerDatagram.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "PlayfieldExitSystem.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ReadyState.hpp"
#include "SequenceOrder.hpp"
#include "SessionConstants.hpp"
#include "SessionDatagram.hpp"
#include "ShipControl.hpp"
#include "TimeConstants.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/**
 * @brief A count of simulation ticks, as a duration: converted to nanoseconds,
 * a whole number of seconds of ticks gives exactly that many seconds.
 */
using SimulationTicks = std::chrono::duration<
    std::uint64_t, std::ratio<1, static_cast<std::intmax_t>(
                                     engine::SIMULATION_TICKS_PER_SECOND)>>;

/**
 * @returns Whether @p input is newer than @p newest: no input was kept yet, or
 * its number is higher. An input numbered like the one kept is not newer: it
 * is that input again, or one the player did not send.
 */
bool isNewer(const network::PlayerInput& input,
             const std::optional<network::PlayerInput>& newest) {
  return !newest.has_value() || input.inputNumber > newest->inputNumber;
}

/**
 * @returns Whether @p input holds other actions than @p newest, or no input
 * was kept yet.
 */
bool changesActions(const network::PlayerInput& input,
                    const std::optional<network::PlayerInput>& newest) {
  return !newest.has_value() || input.actions != newest->actions;
}

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
 * @returns The directions @p input holds for the ship; shooting is not one.
 */
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
      scoreKeeper_(events, roster_),
      worldStateSender_(roster_, send_),
      seed_(settings.seed),
      wavePlan_(std::move(settings.waves)),
      rejectionLog_(std::string{INSTANCES_LOGGER_NAME},
                    network::REJECTION_LINE_INTERVAL),
      logger_(std::string{INSTANCES_LOGGER_NAME}) {
  systems_.add(std::make_unique<game::PatternSystem>());
  systems_.add(std::make_unique<game::MovementSystem>());
  systems_.add(std::make_unique<game::PlayfieldExitSystem>(events));
  systems_.add(std::make_unique<game::CollisionSystem>(events));
  dispatcher_.registerHandler<network::ReadyState>(
      [this](const network::MessageOrigin& origin,
             const network::ReadyState& message) {
        applyReadyState(origin, message);
      });
  dispatcher_.registerHandler<network::PlayerInput>(
      [this](const network::MessageOrigin& origin,
             const network::PlayerInput& input) {
        keepIfNewer(origin, input);
      });
  dispatcher_.registerHandler<network::Heartbeat>(
      [](const network::MessageOrigin& /*origin*/,
         const network::Heartbeat& /*heartbeat*/) {});
  dispatcher_.registerHandler<network::Disconnection>(
      [this](const network::MessageOrigin& origin,
             const network::Disconnection& /*disconnection*/) {
        closeSession(origin.sender, game::DepartureCause::Left);
      });
  startNewWorld();
}

GameState GameInstance::state() const { return state_; }

std::uint32_t GameInstance::tick() const { return tick_; }

PlayerRoster& GameInstance::roster() { return roster_; }

game::World& GameInstance::world() { return *world_; }

network::PacketHeader GameInstance::nextSessionHeaderFor(
    const network::Endpoint& player) {
  return sessionHeaderFor(roster_, player);
}

void GameInstance::onPlayerJoined(ConnectedPlayer& player) {
  shipSpawnAnnouncer_->onPlayerJoined(player);
}

void GameInstance::onTick(
    std::span<const network::IncomingDatagram> datagrams) {
  for (const network::IncomingDatagram& datagram : datagrams) {
    readPlayerDatagram(datagram);
  }
  closeSilentSessions();
  finishIfOver();
  advanceState();
  if (state_ == GameState::InProgress) {
    worldStateSender_.onTick(tick_, world_->components());
  }
  sendHeartbeats();
  ++ticksRun_;
}

void GameInstance::advanceState() {
  switch (state_) {
    case GameState::WaitingForPlayers:
      if (everyPlayerIsReady()) {
        startGame();
      } else {
        lobbyStateAnnouncer_->onWaitingTick();
      }
      break;
    case GameState::InProgress:
      ++tick_;
      moveShips();
      systems_.run(world_->components(), engine::SIMULATION_TICK_DURATION);
      missileLauncher_->onTick(shootersIn(roster_));
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

std::vector<ConnectedPlayer> GameInstance::takeDepartedPlayers() {
  return std::exchange(departedPlayers_, {});
}

void GameInstance::finish() {
  if (state_ == GameState::InProgress) {
    state_ = GameState::Finished;
    finishedTicks_ = 0;
  }
}

void GameInstance::readPlayerDatagram(
    const network::IncomingDatagram& datagram) {
  const std::optional<PlayerDatagram> readable = playerDatagramOf(datagram);
  if (!readable.has_value()) {
    return;
  }
  readable->player->ticksSinceReceived = 0;
  dispatcher_.dispatch(readable->origin, readable->messages);
}

std::optional<PlayerDatagram> GameInstance::playerDatagramOf(
    const network::IncomingDatagram& datagram) {
  network::PacketHeader header;
  std::vector<network::MessageFrame> messages;
  try {
    header = network::DatagramCodec::decodeHeader(datagram.payload);
    if (!network::DatagramCodec::speaksThisProtocol(header)) {
      drop(datagram.sender, network::FOREIGN_HEADER_DROP_REASON);
      return std::nullopt;
    }
    messages = network::DatagramCodec::splitMessages(datagram.payload);
  } catch (const network::NetworkException& error) {
    drop(datagram.sender, error.what());
    return std::nullopt;
  }
  ConnectedPlayer* const player = roster_.find(datagram.sender);
  if (player == nullptr) {
    drop(datagram.sender, NOT_A_PLAYER_DROP_REASON);
    return std::nullopt;
  }
  return PlayerDatagram{
      .player = player,
      .origin =
          network::MessageOrigin{.sender = datagram.sender, .header = header},
      .messages = std::move(messages)};
}

void GameInstance::drop(const network::Endpoint& sender,
                        std::string_view reason) {
  rejectionLog_.debug(timeRun(), "dropped a datagram from ", sender.address,
                      " port ", sender.port, ": ", reason);
}

std::chrono::nanoseconds GameInstance::timeRun() const {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
      SimulationTicks{ticksRun_});
}

void GameInstance::applyReadyState(const network::MessageOrigin& origin,
                                   const network::ReadyState& message) {
  ConnectedPlayer* const player = roster_.find(origin.sender);
  if (state_ != GameState::WaitingForPlayers || player == nullptr) {
    return;
  }
  if (player->newestReceivedSequence.has_value() &&
      !network::isNewerSequence(origin.header.sequence,
                                *player->newestReceivedSequence)) {
    return;
  }
  player->newestReceivedSequence = origin.header.sequence;
  player->ready = message.ready;
}

void GameInstance::keepIfNewer(const network::MessageOrigin& origin,
                               const network::PlayerInput& input) {
  ConnectedPlayer* const player = roster_.find(origin.sender);
  if (player == nullptr || !isNewer(input, player->newestInput)) {
    return;
  }
  if (changesActions(input, player->newestInput)) {
    logger_.debug("player ", static_cast<unsigned int>(player->playerNumber),
                  " holds actions ", static_cast<unsigned int>(input.actions));
  }
  player->newestInput = input;
}

bool GameInstance::everyPlayerIsReady() {
  const ConnectedPlayers players = roster_.connected();
  return !players.empty() &&
         std::ranges::all_of(players, [](const ConnectedPlayer* player) {
           return player->ready;
         });
}

void GameInstance::closeSilentSessions() {
  for (const network::Endpoint& player : countSilentTicks()) {
    closeSession(player, game::DepartureCause::TimedOut);
  }
}

std::vector<network::Endpoint> GameInstance::countSilentTicks() {
  std::vector<network::Endpoint> timedOut;
  for (ConnectedPlayer* const player : roster_.connected()) {
    if (player->ticksSinceReceived >= SESSION_TIMEOUT_TICKS) {
      timedOut.push_back(player->endpoint);
    } else {
      ++player->ticksSinceReceived;
    }
  }
  return timedOut;
}

void GameInstance::closeSession(const network::Endpoint& player,
                                game::DepartureCause cause) {
  std::optional<ConnectedPlayer> departed = roster_.remove(player);
  if (!departed.has_value()) {
    return;
  }
  if (world_->removePlayer(departed->shipEntityId)) {
    announceShipRemoval(departed->shipEntityId);
  }
  announceDeparture(*departed, cause);
  departedPlayers_.push_back(std::move(*departed));
}

void GameInstance::announceShipRemoval(std::uint32_t shipEntityId) {
  events_.publish(game::EntityDestruction{
      .networkId = shipEntityId, .cause = game::DestructionCause::Removed});
}

void GameInstance::announceDeparture(const ConnectedPlayer& player,
                                     game::DepartureCause cause) {
  logDeparture(player, cause);
  events_.publish(game::PlayerDeparture{.playerNumber = player.playerNumber,
                                        .cause = cause});
}

void GameInstance::logDeparture(const ConnectedPlayer& player,
                                game::DepartureCause cause) const {
  const auto playerNumber = static_cast<unsigned int>(player.playerNumber);
  if (cause == game::DepartureCause::Left) {
    logger_.info("player ", playerNumber, " left from ",
                 player.endpoint.address, " port ", player.endpoint.port);
    return;
  }
  logger_.info("player ", playerNumber, " timed out: nothing received from ",
               player.endpoint.address, " port ", player.endpoint.port, " for ",
               network::SESSION_TIMEOUT.count(), " s");
}

void GameInstance::finishIfOver() {
  const std::optional<std::string_view> reason =
      reasonToFinish(state_, roster_);
  if (!reason.has_value()) {
    return;
  }
  logger_.info("game finished: ", *reason);
  finish();
  gameEndAnnouncer_ = std::make_unique<GameEndAnnouncer>(roster_, send_);
}

void GameInstance::sendHeartbeats() {
  for (ConnectedPlayer* const player : roster_.connected()) {
    if (player->ticksSinceSent >= TICKS_PER_HEARTBEAT) {
      send_(sessionDatagram(*player, network::Heartbeat{}));
    }
    ++player->ticksSinceSent;
  }
}

void GameInstance::startGame() {
  state_ = GameState::InProgress;
  const ConnectedPlayers players = roster_.connected();
  for (ConnectedPlayer* const player : players) {
    send_(sessionDatagram(*player, network::GameStarted{}));
  }
  logger_.info("game started, players connected: ", players.size());
}

void GameInstance::moveShips() {
  for (const ConnectedPlayer* const player : roster_.connected()) {
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
      std::make_unique<ShipSpawnAnnouncer>(*world_, roster_, send_);
  bydoSpawner_ = std::make_unique<BydoSpawner>(
      BydoSpawnerParts{.world = *world_,
                       .roster = roster_,
                       .send = send_,
                       .seed = seed_ + worldsBuilt_,
                       .waves = wavePlan_});
  missileLauncher_ = std::make_unique<MissileLauncher>(*world_, roster_, send_);
  bydoGunner_ = std::make_unique<BydoGunner>(*world_, roster_, send_);
  lifeKeeper_ = std::make_unique<LifeKeeper>(
      LifeKeeperParts{.events = events_,
                      .world = *world_,
                      .roster = roster_,
                      .respawn = [this](ConnectedPlayer& player) {
                        shipSpawnAnnouncer_->onPlayerJoined(player);
                      }});
  lobbyStateAnnouncer_ = std::make_unique<LobbyStateAnnouncer>(roster_, send_);
  gameEndAnnouncer_.reset();
}

void GameInstance::startNewWorld() {
  buildWorld();
  ++worldsBuilt_;
  tick_ = 0;
  finishedTicks_ = 0;
  for (ConnectedPlayer* const player : roster_.connected()) {
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
