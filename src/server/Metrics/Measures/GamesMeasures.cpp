#include "GamesMeasures.hpp"
#include <algorithm>
#include <memory>
#include <vector>
#include "ConnectedPlayers.hpp"
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "Games.hpp"
#include "GamesCensus.hpp"
#include "IClock.hpp"
#include "PlayerRoster.hpp"
#include "SystemScheduler.hpp"
#include "SystemTime.hpp"

namespace rtype::server {

namespace {

/** @brief Adds @p time to the total of its name in @p totals, or lists it when
 * the name is new. */
void addTo(std::vector<engine::SystemTime>& totals,
           const engine::SystemTime& time) {
  const auto found =
      std::ranges::find(totals, time.name, &engine::SystemTime::name);
  if (found == totals.end()) {
    totals.push_back(time);
    return;
  }
  found->total += time.total;
}

}  // namespace

GamesCensus censusOf(const Games& games) {
  GamesCensus census{.hosted = games.all().size()};
  for (const std::unique_ptr<GameInstance>& game : games.all()) {
    if (game->state() == GameState::InProgress) {
      ++census.inProgress;
    }
    census.players += game->roster().connected().size();
    census.entities += game->entityCount();
  }
  return census;
}

void measureSystemsOf(const Games& games, const engine::IClock* clock) {
  for (const std::unique_ptr<GameInstance>& game : games.all()) {
    game->systems().measureWith(clock);
  }
}

std::vector<engine::SystemTime> takeSystemTimesOf(const Games& games) {
  std::vector<engine::SystemTime> totals;
  for (const std::unique_ptr<GameInstance>& game : games.all()) {
    for (const engine::SystemTime& time : game->systems().takeSystemTimes()) {
      addTo(totals, time);
    }
  }
  return totals;
}

}  // namespace rtype::server
