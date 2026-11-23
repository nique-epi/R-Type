#include "ScoreKeeper.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include "Bounties.hpp"
#include "ConnectedPlayer.hpp"
#include "EnemyKilled.hpp"
#include "EventBus.hpp"
#include "GameRuleConstants.hpp"
#include "PlayerRoster.hpp"
#include "ScoreChanged.hpp"
#include "SeatIndex.hpp"

namespace rtype::server {

namespace {

using PilotPoints = std::array<std::uint32_t, identifiers::MAXIMUM_PLAYERS>;

/** @returns The points of @p kill each pilot earns, by seat (seatIndexOf()),
 * as ScoreKeeper says. */
PilotPoints sharesOf(const game::EnemyKilled& kill) {
  std::uint64_t total = 0;
  for (const std::uint32_t damage : kill.damageByPilot) {
    total += damage;
  }
  PilotPoints shares{};
  std::uint64_t given = 0;
  for (std::size_t pilot = 0; pilot < shares.size() && total > 0; ++pilot) {
    shares.at(pilot) = static_cast<std::uint32_t>(
        (std::uint64_t{kill.points} * kill.damageByPilot.at(pilot)) / total);
    given += shares.at(pilot);
  }
  const std::size_t killer =
      std::size_t{kill.playerNumber} - identifiers::FIRST_PLAYER_NUMBER;
  if (killer < shares.size()) {
    shares.at(killer) += static_cast<std::uint32_t>(kill.points - given);
  }
  return shares;
}

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
  const PilotPoints shares = sharesOf(kill);
  for (ConnectedPlayer* const player : roster_.connected()) {
    const std::uint32_t before = player->score;
    player->score = saturatedCount(std::uint64_t{player->score} +
                                   shares.at(seatIndexOf(*player)));
    announceGrowth(events_, *player, before);
  }
  payEveryPlayer(roster_, kill.bounty);
}

}  // namespace rtype::server
