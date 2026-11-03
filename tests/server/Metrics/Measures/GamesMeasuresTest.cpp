#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GamePlayers.hpp"
#include "Games.hpp"
#include "GamesCensus.hpp"
#include "GamesMeasures.hpp"
#include "Heartbeat.hpp"
#include "Metrics/ServerMetricsFixture.hpp"
#include "Metrics/ServerMetricsTestConstants.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "RoundTripFigures.hpp"
#include "ServerModuleTestConstants.hpp"
#include "SystemTime.hpp"

using rtype::server::censusOf;
using rtype::server::ConnectedPlayer;
using rtype::server::Games;
using rtype::server::GamesCensus;
using rtype::server::PlayerRoundTripMeasure;
using rtype::server::roundTripsOf;
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

/**
 * Given three games, a player of the second game measured at a round trip of
 * 42 ms
 * When the round trips of the games are gathered
 * Then that player is listed with the number of its game, 2, its number, its
 * nickname and its figures
 */
TEST_F(ServerMetricsFixture, ListsTheRoundTripOfEveryPlayerMeasured) {
  startWithGames(THREE_GAMES);
  const Games& games = *probe().games();
  ConnectedPlayer& player = seatPlayerIn(*games.all()[SECOND_GAME], 0);
  player.roundTrip.receive(
      rtype::network::Heartbeat{
          .sentAt = 0,
          .echo = rtype::network::HeartbeatEcho{.sentAt = 0, .heldFor = 0}},
      MEASURED_ROUND_TRIP);

  const std::vector<PlayerRoundTripMeasure> measures = roundTripsOf(games);

  ASSERT_EQ(measures.size(), 1U);
  EXPECT_EQ(measures.front().gameNumber, NUMBER_OF_THE_SECOND_GAME);
  EXPECT_EQ(measures.front().playerNumber, player.playerNumber);
  EXPECT_EQ(measures.front().nickname, player.nickname);
  EXPECT_EQ(measures.front().figures,
            (rtype::network::RoundTripFigures{
                .roundTrip = MEASURED_ROUND_TRIP,
                .jitter = std::chrono::microseconds::zero()}));
}

/**
 * Given a game whose only player was measured nothing yet
 * When the round trips of the games are gathered
 * Then nobody is listed
 */
TEST_F(ServerMetricsFixture, LeavesOutAPlayerNotMeasuredYet) {
  startWithGames(THREE_GAMES);
  join(0);

  EXPECT_TRUE(roundTripsOf(*probe().games()).empty());
}
