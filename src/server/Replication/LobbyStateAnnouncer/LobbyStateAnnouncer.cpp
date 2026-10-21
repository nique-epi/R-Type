#include "LobbyStateAnnouncer.hpp"
#include <cstddef>
#include <span>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "LobbyState.hpp"
#include "MessagePayload.hpp"
#include "PlayerRoster.hpp"
#include "ServerConstants.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The room @p players make: each of them, in the order given, with
 * its number, its ready state and its nickname.
 */
network::LobbyState roomOf(std::span<ConnectedPlayer* const> players) {
  network::LobbyState room;
  room.players.reserve(players.size());
  for (const ConnectedPlayer* const player : players) {
    room.players.push_back(
        network::LobbyPlayer{.playerNumber = player->playerNumber,
                             .ready = player->ready,
                             .nickname = player->nickname});
  }
  return room;
}

}  // namespace

LobbyStateAnnouncer::LobbyStateAnnouncer(PlayerRoster& roster, Send send)
    : roster_(&roster), send_(std::move(send)) {}

void LobbyStateAnnouncer::onWaitingTick() {
  const std::span<ConnectedPlayer* const> players = roster_->connected();
  network::LobbyState room = roomOf(players);
  ++ticksSinceLastSend_;
  if (room == lastSent_ && ticksSinceLastSend_ < TICKS_PER_LOBBY_STATE) {
    return;
  }
  const std::vector<std::byte> payload = network::encodeMessage(room);
  for (ConnectedPlayer* const player : players) {
    send_(sessionDatagram(*player, network::LobbyState::TYPE, payload));
  }
  lastSent_ = std::move(room);
  ticksSinceLastSend_ = 0;
}

}  // namespace rtype::server
