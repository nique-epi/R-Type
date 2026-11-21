#include "ExtraLifeKeeper.hpp"
#include <algorithm>
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "EventBus.hpp"
#include "ExtraLifeKeeperParts.hpp"
#include "ExtraLifeWon.hpp"
#include "ExtraLives.hpp"
#include "GameRuleConstants.hpp"
#include "PlayerLives.hpp"
#include "PlayerRoster.hpp"
#include "ScoreChanged.hpp"

namespace rtype::server {

ExtraLifeKeeper::ExtraLifeKeeper(ExtraLifeKeeperParts parts)
    : parts_(parts),
      subscription_(parts_.events.get().subscribe<game::ScoreChanged>(
          [this](const game::ScoreChanged& change) {
            onScoreChanged(change);
          })) {}

ExtraLifeKeeper::~ExtraLifeKeeper() {
  parts_.events.get().unsubscribe(subscription_);
}

void ExtraLifeKeeper::onScoreChanged(const game::ScoreChanged& change) {
  PlayerRoster& roster = parts_.roster.get();
  ConnectedPlayer* const player = roster.findByNumber(change.playerNumber);
  if (player == nullptr || livesOf(parts_.world.get(), *player) == 0) {
    return;
  }
  if (giveLives(*player, extraLivesWonBy(change))) {
    parts_.events.get().publish(
        game::ExtraLifeWon{.playerNumber = player->playerNumber});
  }
}

bool ExtraLifeKeeper::giveLives(ConnectedPlayer& player,
                                std::uint32_t won) const {
  const std::uint8_t held = livesOf(parts_.world.get(), player);
  const auto lives = static_cast<std::uint8_t>(
      std::min<std::uint32_t>(held + won, identifiers::HIGHEST_LIVES));
  setLivesOf(parts_.world.get(), player, lives);
  return lives > held;
}

}  // namespace rtype::server
