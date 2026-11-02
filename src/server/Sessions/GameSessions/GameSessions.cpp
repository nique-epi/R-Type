#include "GameSessions.hpp"
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "GameSessionsParts.hpp"
#include "Heartbeat.hpp"
#include "Heartbeats.hpp"
#include "IClock.hpp"
#include "IncomingDatagram.hpp"
#include "MessageOrigin.hpp"
#include "PacketHeader.hpp"
#include "PlayerDatagram.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "ReadyState.hpp"
#include "SessionDatagram.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

GameSessions::GameSessions(GameSessionsParts parts)
    : send_(std::move(parts.send)),
      clock_(&parts.clock.get()),
      previousTickStart_(clock_->now()),
      reader_(roster_),
      readyKeeper_(roster_, std::move(parts.readyAccepted)),
      inputKeeper_(roster_, std::move(parts.onNewerInput)),
      departures_(roster_, parts.events, std::move(parts.onClosing)) {
  dispatcher_.registerHandler<network::ReadyState>(
      [this](const network::MessageOrigin& origin,
             const network::ReadyState& message) {
        readyKeeper_.apply(origin, message);
      });
  dispatcher_.registerHandler<network::PlayerInput>(
      [this](const network::MessageOrigin& origin,
             const network::PlayerInput& input) {
        inputKeeper_.keepIfNewer(origin, input);
      });
  dispatcher_.registerHandler<network::Heartbeat>(
      [](const network::MessageOrigin&, const network::Heartbeat&) {});
  dispatcher_.registerHandler<network::Disconnection>(
      [this](const network::MessageOrigin& origin,
             const network::Disconnection&) {
        departures_.close(origin.sender, game::DepartureCause::Left);
      });
}

PlayerRoster& GameSessions::roster() { return roster_; }

void GameSessions::readDatagrams(
    std::span<const network::IncomingDatagram> datagrams) {
  for (const network::IncomingDatagram& datagram : datagrams) {
    readDatagram(datagram);
  }
}

void GameSessions::closeSilentSessions() { departures_.closeSilentSessions(); }

bool GameSessions::everyPlayerIsReady() {
  return readyKeeper_.everyPlayerIsReady();
}

void GameSessions::startTick() {
  const engine::Duration tickStart = clock_->now();
  addTimeSinceSent(roster_,
                   tickStart - std::exchange(previousTickStart_, tickStart));
}

void GameSessions::endTick() {
  sendHeartbeats(roster_, send_);
  reader_.countTick();
}

network::PacketHeader GameSessions::nextSessionHeaderFor(
    const network::Endpoint& player) {
  return sessionHeaderFor(roster_, player);
}

std::vector<ConnectedPlayer> GameSessions::takeDepartedPlayers() {
  return departures_.takeDeparted();
}

void GameSessions::readDatagram(const network::IncomingDatagram& datagram) {
  const std::optional<PlayerDatagram> readable = reader_.read(datagram);
  if (!readable.has_value()) {
    return;
  }
  readable->player->ticksSinceReceived = 0;
  readable->player->away = false;
  dispatcher_.dispatch(readable->origin, readable->messages);
}

}  // namespace rtype::server
