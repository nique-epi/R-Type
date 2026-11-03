#include "LeaderboardDesk.hpp"
#include <cstddef>
#include <functional>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "GameInstanceConstants.hpp"
#include "IResultsArchive.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardAsk.hpp"
#include "LeaderboardReply.hpp"
#include "LeaderboardRequest.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

LeaderboardDesk::LeaderboardDesk(
    PlayerRoster& roster, IResultsArchive& archive,
    std::function<void(network::OutgoingDatagram)> send)
    : roster_(roster),
      archive_(archive),
      send_(std::move(send)),
      replies_(network::MAXIMUM_PLAYERS) {
  repliesToSend_.reserve(network::MAXIMUM_PLAYERS);
}

void LeaderboardDesk::onAsked(const ConnectedPlayer& player,
                              const network::LeaderboardRequest& request) {
  std::size_t& nextAnswerTick = nextAnswerTicks_.at(static_cast<std::size_t>(
      player.playerNumber - network::FIRST_PLAYER_NUMBER));
  if (ticks_ < nextAnswerTick) {
    return;
  }
  nextAnswerTick = ticks_ + TICKS_BETWEEN_LEADERBOARD_ANSWERS;
  archive_.ask(LeaderboardAsk{.request = request,
                              .nickname = player.nickname,
                              .asker = player.endpoint,
                              .sessionToken = player.sessionToken,
                              .replies = &replies_});
}

void LeaderboardDesk::onTick() {
  ++ticks_;
  replies_.drainInto(repliesToSend_);
  for (const LeaderboardReply& reply : repliesToSend_) {
    deliver(reply);
  }
  repliesToSend_.clear();
}

void LeaderboardDesk::deliver(const LeaderboardReply& reply) {
  ConnectedPlayer* const player = roster_.find(reply.recipient);
  if (player == nullptr || player->sessionToken != reply.sessionToken) {
    return;
  }
  send_(sessionDatagram(*player, network::LeaderboardAnswer::TYPE,
                        reply.payload));
}

}  // namespace rtype::server
