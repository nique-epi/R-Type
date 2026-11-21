#include "ExtraLifeKeeper.hpp"
#include <algorithm>
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "EnemyKilled.hpp"
#include "EventBus.hpp"
#include "ExtraLifeKeeperParts.hpp"
#include "ExtraLifeWon.hpp"
#include "ExtraLives.hpp"
#include "GameRuleConstants.hpp"
#include "Lives.hpp"
#include "PlayerLives.hpp"
#include "PlayerRoster.hpp"
#include "ScoreChange.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @returns The score of @p player before a kill of @p points, which the
 * ScoreKeeper added already: never under 0. */
ScoreChange scoreChangeOf(const ConnectedPlayer& player, std::uint32_t points) {
  return ScoreChange{
      .before = player.score > points ? player.score - points : 0,
      .after = player.score};
}

}  // namespace

ExtraLifeKeeper::ExtraLifeKeeper(ExtraLifeKeeperParts parts)
    : parts_(parts),
      subscription_(parts_.events.get().subscribe<game::EnemyKilled>(
          [this](const game::EnemyKilled& kill) { onEnemyKilled(kill); })) {}

ExtraLifeKeeper::~ExtraLifeKeeper() {
  parts_.events.get().unsubscribe(subscription_);
}

void ExtraLifeKeeper::onEnemyKilled(const game::EnemyKilled& kill) {
  ConnectedPlayer* const player =
      parts_.roster.get().findByNumber(kill.playerNumber);
  if (player == nullptr || livesOf(parts_.world.get(), *player) == 0) {
    return;
  }
  const std::uint32_t won =
      extraLivesWonBy(scoreChangeOf(*player, kill.points));
  if (won > 0 && giveLives(*player, won)) {
    parts_.events.get().publish(
        game::ExtraLifeWon{.playerNumber = player->playerNumber});
  }
}

bool ExtraLifeKeeper::giveLives(ConnectedPlayer& player, std::uint32_t won) {
  const std::uint8_t held = livesOf(parts_.world.get(), player);
  const auto lives = static_cast<std::uint8_t>(
      std::min<std::uint32_t>(held + won, identifiers::HIGHEST_LIVES));
  player.livesWithoutShip = lives;
  if (auto* const ship = parts_.world.get().shipComponentOf<game::Lives>(
          player.shipEntityId)) {
    ship->count = lives;
  }
  return lives > held;
}

}  // namespace rtype::server
