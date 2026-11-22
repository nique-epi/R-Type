#include "ScoreKeeper.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include "Bounties.hpp"
#include "ConnectedPlayer.hpp"
#include "EnemyKilled.hpp"
#include "EventBus.hpp"
#include "PilotCountConstants.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

static_assert(game::MOST_PILOTS == network::MAXIMUM_PLAYERS,
              "the tally of a boss holds a place for every player");
static_assert(game::FIRST_PILOT_NUMBER == network::FIRST_PLAYER_NUMBER,
              "pilot n of a tally is player n");

namespace {

using PilotPoints = std::array<std::uint32_t, game::MOST_PILOTS>;

/** @returns The points of @p kill each pilot earns, pilot
 * game::FIRST_PILOT_NUMBER first, as ScoreKeeper says. */
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
  const std::size_t killer = kill.playerNumber - game::FIRST_PILOT_NUMBER;
  if (killer < shares.size()) {
    shares.at(killer) += static_cast<std::uint32_t>(kill.points - given);
  }
  return shares;
}

/** @brief Adds @p points to the score of @p player, stopping at its largest
 * value; nothing for no player. */
void addScore(ConnectedPlayer* player, std::uint32_t points) {
  if (player == nullptr) {
    return;
  }
  const std::uint32_t room =
      std::numeric_limits<std::uint32_t>::max() - player->score;
  player->score += points < room ? points : room;
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
  for (std::size_t pilot = 0; pilot < shares.size(); ++pilot) {
    addScore(roster_.findByNumber(
                 static_cast<std::uint8_t>(game::FIRST_PILOT_NUMBER + pilot)),
             shares.at(pilot));
  }
  if (kill.bounty > 0) {
    payEveryPlayer(roster_, kill.bounty);
  }
}

}  // namespace rtype::server
