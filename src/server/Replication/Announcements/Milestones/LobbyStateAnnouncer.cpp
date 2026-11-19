#include "LobbyStateAnnouncer.hpp"
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "LobbyState.hpp"
#include "MessagePayload.hpp"
#include "PlayerRoster.hpp"
#include "ServerConstants.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The room @p players make: each of them, in the order given, with
 * its number, its ready state, its nickname and its map vote, then the maps
 * @p maps.
 */
network::LobbyState roomOf(const ConnectedPlayers& players,
                           const std::vector<std::string>& maps) {
  network::LobbyState room{.players = {}, .maps = maps};
  room.players.reserve(players.size());
  for (const ConnectedPlayer* const player : players) {
    room.players.push_back(
        network::LobbyPlayer{.playerNumber = player->playerNumber,
                             .ready = player->ready,
                             .nickname = player->nickname,
                             .mapVote = player->mapVote});
  }
  return room;
}

}  // namespace

LobbyStateAnnouncer::LobbyStateAnnouncer(PlayerRoster& roster, Send send,
                                         std::vector<std::string> maps)
    : roster_(&roster), send_(std::move(send)), maps_(std::move(maps)) {}

void LobbyStateAnnouncer::onWaitingTick() {
  const ConnectedPlayers players = roster_->connected();
  network::LobbyState room = roomOf(players, maps_);
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
