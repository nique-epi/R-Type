#include "GameInstance.hpp"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <span>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "EntityIdConstants.hpp"
#include "GameInstanceConstants.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "MessageOrigin.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "ReadyState.hpp"
#include "SequenceOrder.hpp"
#include "World.hpp"

namespace rtype::server {

GameInstance::GameInstance(Send send) : send_(std::move(send)) {
  dispatcher_.registerHandler<network::ReadyState>(
      [this](const network::MessageOrigin& origin,
             const network::ReadyState& message) {
        applyReadyState(origin, message);
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
  readReadyStates(datagrams);
  switch (state_) {
    case GameState::WaitingForPlayers:
      if (everyPlayerIsReady()) {
        state_ = GameState::InProgress;
      }
      break;
    case GameState::InProgress:
      ++tick_;
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

void GameInstance::readReadyStates(
    std::span<const network::IncomingDatagram> datagrams) {
  for (const network::IncomingDatagram& datagram : datagrams) {
    try {
      const network::PacketHeader header =
          network::DatagramCodec::decodeHeader(datagram.payload);
      if (!network::DatagramCodec::speaksThisProtocol(header)) {
        continue;
      }
      dispatcher_.dispatch(
          network::MessageOrigin{.sender = datagram.sender, .header = header},
          network::DatagramCodec::splitMessages(datagram.payload));
    } catch (const network::NetworkException&) {
      continue;
    }
  }
}

void GameInstance::applyReadyState(const network::MessageOrigin& origin,
                                   const network::ReadyState& message) {
  ConnectedPlayer* const player = roster_.find(origin.sender);
  if (state_ != GameState::WaitingForPlayers || player == nullptr ||
      origin.header.sessionToken != player->sessionToken) {
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
  tick_ = 0;
  finishedTicks_ = 0;
  for (ConnectedPlayer* const player : roster_.connected()) {
    player->ready = false;
    player->shipEntityId = identifiers::NO_ENTITY_ID;
    shipSpawnAnnouncer_->onPlayerJoined(*player);
  }
}

}  // namespace rtype::server
