#include "GameSessions.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "ChatLine.hpp"
#include "ConnectedPlayer.hpp"
#include "DatagramReader.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "GameLoaded.hpp"
#include "GameSessionsParts.hpp"
#include "Heartbeat.hpp"
#include "HeartbeatMoment.hpp"
#include "Heartbeats.hpp"
#include "IClock.hpp"
#include "IncomingDatagram.hpp"
#include "LeaderboardRequest.hpp"
#include "MessageDelivery.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "PacketHeader.hpp"
#include "PlayerDatagram.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "ReadyState.hpp"
#include "SessionDatagram.hpp"
#include "ShipLookChoice.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @brief Reads @p datagram when @p reader takes it (see DatagramReader): the
 * silence of its sender starts over, the player is no longer away, the
 * guaranteed messages and the states its header acknowledges are dropped, then
 * @p dispatcher hands its messages to their handlers.
 */
void readDatagram(DatagramReader& reader,
                  network::MessageDispatcher& dispatcher,
                  const network::IncomingDatagram& datagram) {
  const std::optional<PlayerDatagram> readable = reader.read(datagram);
  if (!readable.has_value()) {
    return;
  }
  readable->player->ticksSinceReceived = 0;
  readable->player->away = false;
  readable->player->unacknowledged.acknowledge(readable->origin.header);
  readable->player->sentStates.acknowledge(readable->origin.header);
  dispatcher.dispatch(readable->origin, readable->messages);
}

}  // namespace

GameSessions::GameSessions(GameSessionsParts parts)
    : send_(std::move(parts.send)),
      clock_(&parts.clock.get()),
      previousTickStart_(clock_->now()),
      reader_(roster_),
      readyKeeper_(roster_, std::move(parts.readyAccepted)),
      inputKeeper_(roster_, std::move(parts.onNewerInput)),
      departures_(roster_, parts.events, std::move(parts.onClosing)),
      onLeaderboardRequest_(std::move(parts.onLeaderboardRequest)),
      onChatLine_(std::move(parts.onChatLine)),
      onShipLookChoice_(std::move(parts.onShipLookChoice)) {
  registerSessionHandlers();
  registerRequestHandlers();
}

void GameSessions::registerSessionHandlers() {
  dispatcher_.registerHandler<network::ReadyState>(
      [this](const network::MessageOrigin& origin,
             const network::ReadyState& message) {
        readyKeeper_.apply(origin, message);
      });
  dispatcher_.registerHandler<network::GameLoaded>(
      [this](const network::MessageOrigin& origin,
             const network::GameLoaded& message) {
        readyKeeper_.apply(origin, message);
      });
  dispatcher_.registerHandler<network::PlayerInput>(
      [this](const network::MessageOrigin& origin,
             const network::PlayerInput& input) {
        inputKeeper_.keepNewerOf(origin, input);
      });
  dispatcher_.registerHandler<network::Heartbeat>(
      [this](const network::MessageOrigin& origin,
             const network::Heartbeat& heartbeat) {
        readHeartbeat(roster_, origin, heartbeat);
      });
  dispatcher_.registerHandler<network::Disconnection>(
      [this](const network::MessageOrigin& origin,
             const network::Disconnection&) {
        departures_.close(origin.sender, game::DepartureCause::Left);
      });
}

void GameSessions::registerRequestHandlers() {
  dispatcher_.registerHandler<network::LeaderboardRequest>(
      [this](const network::MessageOrigin& origin,
             const network::LeaderboardRequest& request) {
        const ConnectedPlayer* const player = roster_.find(origin.sender);
        if (player != nullptr) {
          onLeaderboardRequest_(*player, request);
        }
      });
  dispatcher_.registerHandler<network::ChatLine>(
      [this](const network::MessageOrigin& origin,
             const network::ChatLine& line) {
        const ConnectedPlayer* const player = roster_.find(origin.sender);
        if (player != nullptr) {
          onChatLine_(*player, line);
        }
      });
  dispatcher_.registerHandler<network::ShipLookChoice>(
      [this](const network::MessageOrigin& origin,
             const network::ShipLookChoice& choice) {
        ConnectedPlayer* const player = roster_.find(origin.sender);
        if (player != nullptr && readyKeeper_.acceptsRoomChoices()) {
          onShipLookChoice_(*player, choice);
        }
      });
}

PlayerRoster& GameSessions::roster() { return roster_; }

void GameSessions::readDatagrams(
    std::span<const network::IncomingDatagram> datagrams) {
  for (const network::IncomingDatagram& datagram : datagrams) {
    readDatagram(reader_, dispatcher_, datagram);
  }
}

void GameSessions::closeSilentSessions() { departures_.closeSilentSessions(); }

bool GameSessions::everyPlayerIsReady() {
  return readyKeeper_.everyPlayerIsReady();
}

void GameSessions::startTick() {
  const engine::Duration tickStart = clock_->now();
  const engine::Duration elapsed =
      tickStart - std::exchange(previousTickStart_, tickStart);
  addTimeSinceHeartbeat(roster_, elapsed);
  ageGuaranteedMessages(roster_, elapsed);
}

void GameSessions::endTick(std::optional<std::uint32_t> gameTick) {
  resendGuaranteedMessages(roster_, send_);
  sendHeartbeats(
      roster_, HeartbeatMoment{.now = clock_->now(), .tick = gameTick}, send_);
  reader_.countTick();
}

network::PacketHeader GameSessions::nextSessionHeaderFor(
    const network::Endpoint& player) {
  return sessionHeaderFor(roster_, player);
}

std::vector<ConnectedPlayer> GameSessions::takeDepartedPlayers() {
  return departures_.takeDeparted();
}

}  // namespace rtype::server
