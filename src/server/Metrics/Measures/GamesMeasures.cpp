#include "GamesMeasures.hpp"
#include <algorithm>
#include <optional>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "GamesCensus.hpp"
#include "IClock.hpp"
#include "MetricsConstants.hpp"
#include "PlayerRoster.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "RoundTripFigures.hpp"
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

GamesCensus censusOf(GameInstance& game) {
  return GamesCensus{
      .hosted = GAMES_OF_A_GAME_PROCESS,
      .inProgress = game.state() == GameState::InProgress ? 1U : 0U,
      .players = game.roster().connected().size(),
      .entities = game.entityCount()};
}

void measureSystemsOf(GameInstance& game, const engine::IClock* clock) {
  game.systems().measureWith(clock);
}

std::vector<engine::SystemTime> takeSystemTimesOf(GameInstance& game) {
  std::vector<engine::SystemTime> totals;
  for (const engine::SystemTime& time : game.systems().takeSystemTimes()) {
    addTo(totals, time);
  }
  return totals;
}

std::vector<PlayerRoundTripMeasure> roundTripsOf(GameInstance& game) {
  std::vector<PlayerRoundTripMeasure> measures;
  for (const ConnectedPlayer* const player : game.roster().connected()) {
    const std::optional<network::RoundTripFigures> figures =
        player->roundTrip.figures();
    if (figures.has_value()) {
      measures.push_back(
          PlayerRoundTripMeasure{.gameNumber = NUMBER_OF_THE_GAME,
                                 .playerNumber = player->playerNumber,
                                 .nickname = player->nickname,
                                 .figures = *figures});
    }
  }
  return measures;
}

}  // namespace rtype::server
