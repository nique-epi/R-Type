#include <gtest/gtest.h>
#include <algorithm>
#include <vector>
#include "GamePlayers.hpp"
#include "Games.hpp"
#include "GamesCensus.hpp"
#include "GamesMeasures.hpp"
#include "ServerMetricsFixture.hpp"
#include "ServerMetricsTestConstants.hpp"
#include "ServerModuleTestConstants.hpp"
#include "SystemTime.hpp"

using rtype::server::censusOf;
using rtype::server::Games;
using rtype::server::GamesCensus;
using rtype::server::takeSystemTimesOf;

/**
 * Given three games nobody joined
 * When they are counted
 * Then three are hosted and they hold nothing
 */
TEST_F(ServerMetricsFixture, TheCensusOfEmptyGamesHoldsOnlyTheGames) {
  startWithGames(THREE_GAMES);

  const GamesCensus census = censusOf(*probe().games());

  EXPECT_EQ(census.hosted, THREE_GAMES);
  EXPECT_EQ(census.inProgress, 0U);
  EXPECT_EQ(census.players, 0U);
  EXPECT_EQ(census.entities, 0U);
}

/**
 * Given two players in the first game and one in the second, nobody ready
 * When the games are counted
 * Then the three players and their three ships are counted, and no game is in
 * progress
 */
TEST_F(ServerMetricsFixture, TheCensusAddsUpThePlayersAndTheShipsOfEveryGame) {
  startWithGames(THREE_GAMES);
  const Games& games = *probe().games();
  join(0);
  join(1);
  seatPlayerIn(*games.all()[SECOND_GAME], 2);

  const GamesCensus census = censusOf(games);

  EXPECT_EQ(census.inProgress, 0U);
  EXPECT_EQ(census.players, 3U);
  EXPECT_EQ(census.entities, 3U);
}

/**
 * Given a game whose only player is ready, while its client still loads
 * When the games are counted
 * Then no game is in progress: its systems do not run yet
 */
TEST_F(ServerMetricsFixture, TheCensusLeavesOutAGameStillStarting) {
  startWithGames(THREE_GAMES);
  join(0);
  sayReady(0);

  EXPECT_EQ(censusOf(*probe().games()).inProgress, 0U);
}

/**
 * Given three games, the first one started by its only player
 * When the games are counted
 * Then one game is in progress
 */
TEST_F(ServerMetricsFixture, TheCensusCountsAGameInProgress) {
  startWithGames(THREE_GAMES);
  join(0);
  startTheGameOfPlayer(0);

  const GamesCensus census = censusOf(*probe().games());

  EXPECT_EQ(census.hosted, THREE_GAMES);
  EXPECT_EQ(census.inProgress, 1U);
}

/**
 * Given two games in progress and one waiting, each with a timed system
 * When a tick runs
 * Then that system is listed once, with the time of the two games in progress
 */
TEST_F(ServerMetricsFixture, AddsUpTheTimeOfASystemOverTheGamesInProgress) {
  startWithGames(THREE_GAMES);
  const Games& games = *probe().games();
  join(0);
  seatPlayerIn(*games.all()[SECOND_GAME], 1);
  startTheGameOfPlayer(0);
  startTheGameOfPlayer(1);
  for (const auto& game : games.all()) {
    addTimedSystemTo(*game);
  }

  runTick({});

  const std::vector<rtype::engine::SystemTime> times = takeSystemTimesOf(games);
  const auto timed = std::ranges::find(times, TIMED_SYSTEM_NAME,
                                       &rtype::engine::SystemTime::name);
  ASSERT_NE(timed, times.end());
  EXPECT_EQ(timed->total, TIMED_SYSTEM_TIME_IN_TWO_GAMES);
}
