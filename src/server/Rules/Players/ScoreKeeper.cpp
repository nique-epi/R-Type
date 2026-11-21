#include "ScoreKeeper.hpp"
#include <cstdint>
#include <limits>
#include "ConnectedPlayer.hpp"
#include "EnemyKilled.hpp"
#include "EventBus.hpp"
#include "PlayerRoster.hpp"
#include "ScoreChanged.hpp"

namespace rtype::server {

namespace {

/** @brief Publishes on @p events the ScoreChanged of @p player, whose score
 * was @p before, when it grew. */
void announceGrowth(engine::EventBus& events, const ConnectedPlayer& player,
                    std::uint32_t before) {
  if (player.score > before) {
    events.publish(game::ScoreChanged{.playerNumber = player.playerNumber,
                                      .before = before,
                                      .after = player.score});
  }
}

}  // namespace

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
  const std::uint32_t before = player->score;
  player->score += kill.points < room ? kill.points : room;
  announceGrowth(events_, *player, before);
}

}  // namespace rtype::server
