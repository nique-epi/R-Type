#include "LobbyStateAnnouncer.hpp"
#include <cstddef>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "LobbyMap.hpp"
#include "LobbyState.hpp"
#include "MessagePayload.hpp"
#include "PlayerRoster.hpp"
#include "ServerConstants.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The lines of the room @p players make: each of them, in the order
 * given, with its number, its ready state, its nickname and its map vote.
 */
std::vector<network::LobbyPlayer> roomOf(const ConnectedPlayers& players) {
  std::vector<network::LobbyPlayer> lines;
  lines.reserve(players.size());
  for (const ConnectedPlayer* const player : players) {
    lines.push_back(network::LobbyPlayer{.playerNumber = player->playerNumber,
                                         .ready = player->ready,
                                         .nickname = player->nickname,
                                         .mapVote = player->mapVote});
  }
  return lines;
}

}  // namespace

LobbyStateAnnouncer::LobbyStateAnnouncer(PlayerRoster& roster, Send send,
                                         std::vector<network::LobbyMap> maps)
    : roster_(&roster),
      send_(std::move(send)),
      lastSent_{.players = {}, .maps = std::move(maps)} {}

void LobbyStateAnnouncer::onWaitingTick() {
  const ConnectedPlayers players = roster_->connected();
  std::vector<network::LobbyPlayer> lines = roomOf(players);
  ++ticksSinceLastSend_;
  if (lines == lastSent_.players &&
      ticksSinceLastSend_ < TICKS_PER_LOBBY_STATE) {
    return;
  }
  lastSent_.players = std::move(lines);
  const std::vector<std::byte> payload = network::encodeMessage(lastSent_);
  for (ConnectedPlayer* const player : players) {
    send_(sessionDatagram(*player, network::LobbyState::TYPE, payload));
  }
  ticksSinceLastSend_ = 0;
}

}  // namespace rtype::server
