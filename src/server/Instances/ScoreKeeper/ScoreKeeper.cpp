#include "ScoreKeeper.hpp"
#include <cstdint>
#include <limits>
#include "ConnectedPlayer.hpp"
#include "EnemyKilled.hpp"
#include "EventBus.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

ScoreKeeper::ScoreKeeper(engine::EventBus& events, PlayerRoster& roster)
    : events_(events),
      roster_(roster),
      subscription_(events.subscribe<game::EnemyKilled>(
          [this](const game::EnemyKilled& kill) { onEnemyKilled(kill); })) {}

ScoreKeeper::~ScoreKeeper() { events_.unsubscribe(subscription_); }

void ScoreKeeper::onEnemyKilled(const game::EnemyKilled& kill) {
  ConnectedPlayer* const player = roster_.findByNumber(kill.playerNumber);
  if (player == nullptr) {
    return;
  }
  const std::uint32_t room =
      std::numeric_limits<std::uint32_t>::max() - player->score;
  player->score += kill.points < room ? kill.points : room;
}

}  // namespace rtype::server
