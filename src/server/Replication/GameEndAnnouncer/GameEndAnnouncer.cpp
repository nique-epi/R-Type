#include "GameEndAnnouncer.hpp"
#include <cstddef>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "EntitySpawnSender.hpp"
#include "GameEnded.hpp"
#include "MessagePayload.hpp"
#include "PlayerRoster.hpp"
#include "ServerConstants.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The result @p players end the game with: each of them, in the
 * order given, with its number and its score.
 */
network::GameEnded resultOf(const ConnectedPlayers& players) {
  network::GameEnded result;
  result.players.reserve(players.size());
  for (const ConnectedPlayer* const player : players) {
    result.players.push_back(network::PlayerScore{
        .playerNumber = player->playerNumber, .score = player->score});
  }
  return result;
}

}  // namespace

GameEndAnnouncer::GameEndAnnouncer(PlayerRoster& roster, SendDatagram send)
    : roster_(&roster),
      send_(std::move(send)),
      result_(resultOf(roster.connected())) {}

void GameEndAnnouncer::onFinishedTick() {
  ++ticksSinceLastSend_;
  if (ticksSinceLastSend_ < TICKS_PER_GAME_ENDED) {
    return;
  }
  const std::vector<std::byte> payload = network::encodeMessage(result_);
  for (ConnectedPlayer* const player : roster_->connected()) {
    send_(sessionDatagram(*player, network::GameEnded::TYPE, payload));
  }
  ticksSinceLastSend_ = 0;
}

}  // namespace rtype::server
