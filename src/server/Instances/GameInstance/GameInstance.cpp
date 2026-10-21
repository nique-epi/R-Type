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
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "EntityIdConstants.hpp"
#include "GameInstanceConstants.hpp"
#include "GameStarted.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MovementSystem.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "PatternSystem.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "PlayfieldExitSystem.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ReadyState.hpp"
#include "SequenceOrder.hpp"
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

}  // namespace

GameInstance::GameInstance(Send send, std::uint32_t seed,
                           engine::EventBus& events)
    : send_(std::move(send)),
      scoreKeeper_(events, roster_),
      worldStateSender_(roster_, send_),
      seed_(seed),
      rejectionLog_(std::string{INSTANCES_LOGGER_NAME},
                    network::REJECTION_LINE_INTERVAL),
      logger_(std::string{INSTANCES_LOGGER_NAME}) {
  systems_.add(std::make_unique<game::PatternSystem>());
  systems_.add(std::make_unique<game::MovementSystem>());
  systems_.add(std::make_unique<game::PlayfieldExitSystem>(events));
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
      bydoSpawner_->onTick();
      break;
    case GameState::Finished:
      ++finishedTicks_;
      if (finishedTicks_ >= FINISHED_STATE_TICKS) {
        startNewWorld();
        state_ = GameState::WaitingForPlayers;
      }
      break;
  }
  if (state_ == GameState::InProgress) {
    worldStateSender_.onTick(tick_, world_->components());
  }
  ++ticksRun_;
}

void GameInstance::finish() {
  if (state_ == GameState::InProgress) {
    state_ = GameState::Finished;
    finishedTicks_ = 0;
  }
}

void GameInstance::readPlayerDatagram(
    const network::IncomingDatagram& datagram) {
  network::PacketHeader header;
  std::vector<network::MessageFrame> messages;
  try {
    header = network::DatagramCodec::decodeHeader(datagram.payload);
    if (!network::DatagramCodec::speaksThisProtocol(header)) {
      drop(datagram.sender,
           "its header is not of this protocol and version, or has a flag set");
      return;
    }
    messages = network::DatagramCodec::splitMessages(datagram.payload);
  } catch (const network::NetworkException& error) {
    drop(datagram.sender, error.what());
    return;
  }
  if (roster_.find(datagram.sender) == nullptr) {
    drop(datagram.sender, "its sender is not a player of this game");
    return;
  }
  dispatcher_.dispatch(
      network::MessageOrigin{.sender = datagram.sender, .header = header},
      messages);
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

void GameInstance::startNewWorld() {
  world_ = std::make_unique<game::World>();
  shipSpawnAnnouncer_ =
      std::make_unique<ShipSpawnAnnouncer>(*world_, roster_, send_);
  bydoSpawner_ = std::make_unique<BydoSpawner>(*world_, roster_, send_,
                                               seed_ + worldsBuilt_);
  lobbyStateAnnouncer_ = std::make_unique<LobbyStateAnnouncer>(roster_, send_);
  ++worldsBuilt_;
  tick_ = 0;
  finishedTicks_ = 0;
  for (ConnectedPlayer* const player : roster_.connected()) {
    player->ready = false;
    player->score = 0;
    player->shipEntityId = identifiers::NO_ENTITY_ID;
    shipSpawnAnnouncer_->onPlayerJoined(*player);
  }
}

}  // namespace rtype::server
