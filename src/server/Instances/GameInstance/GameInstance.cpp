#include "GameInstance.hpp"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "EntityIdConstants.hpp"
#include "GameInstanceConstants.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MovementSystem.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "PatternSystem.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "PlayfieldExitSystem.hpp"
#include "ReadyState.hpp"
#include "SequenceOrder.hpp"
#include "TimeConstants.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

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

}  // namespace

GameInstance::GameInstance(Send send, std::uint32_t seed,
                           engine::EventBus& events)
    : send_(std::move(send)),
      seed_(seed),
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
        state_ = GameState::InProgress;
      }
      break;
    case GameState::InProgress:
      ++tick_;
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
}

void GameInstance::finish() {
  if (state_ == GameState::InProgress) {
    state_ = GameState::Finished;
    finishedTicks_ = 0;
  }
}

void GameInstance::readPlayerDatagram(
    const network::IncomingDatagram& datagram) {
  try {
    const network::PacketHeader header =
        network::DatagramCodec::decodeHeader(datagram.payload);
    if (!network::DatagramCodec::speaksThisProtocol(header)) {
      return;
    }
    const std::vector<network::MessageFrame> messages =
        network::DatagramCodec::splitMessages(datagram.payload);
    const ConnectedPlayer* const player = roster_.find(datagram.sender);
    if (player == nullptr) {
      return;
    }
    dispatcher_.dispatch(
        network::MessageOrigin{.sender = datagram.sender, .header = header},
        messages);
  } catch (const network::NetworkException&) {
    return;
  }
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
  const std::vector<ConnectedPlayer*> players = roster_.connected();
  return !players.empty() &&
         std::ranges::all_of(players, [](const ConnectedPlayer* player) {
           return player->ready;
         });
}

void GameInstance::startNewWorld() {
  world_ = std::make_unique<game::World>();
  shipSpawnAnnouncer_ =
      std::make_unique<ShipSpawnAnnouncer>(*world_, roster_, send_);
  bydoSpawner_ = std::make_unique<BydoSpawner>(*world_, roster_, send_,
                                               seed_ + worldsBuilt_);
  ++worldsBuilt_;
  tick_ = 0;
  finishedTicks_ = 0;
  for (ConnectedPlayer* const player : roster_.connected()) {
    player->ready = false;
    player->shipEntityId = identifiers::NO_ENTITY_ID;
    shipSpawnAnnouncer_->onPlayerJoined(*player);
  }
}

}  // namespace rtype::server
