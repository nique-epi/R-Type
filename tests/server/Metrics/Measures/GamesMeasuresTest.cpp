#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameInstance.hpp"
#include "GamesCensus.hpp"
#include "GamesMeasures.hpp"
#include "Heartbeat.hpp"
#include "Metrics/ServerMetricsFixture.hpp"
#include "Metrics/ServerMetricsTestConstants.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "RoundTripFigures.hpp"
#include "SystemTime.hpp"

using rtype::server::censusOf;
using rtype::server::ConnectedPlayer;
using rtype::server::GameInstance;
using rtype::server::GamesCensus;
using rtype::server::PlayerRoundTripMeasure;
using rtype::server::roundTripsOf;
using rtype::server::takeSystemTimesOf;

/**
 * Given a game nobody joined
 * When it is counted
 * Then the one game of the process is hosted and it holds nothing
 */
TEST_F(ServerMetricsFixture, TheCensusOfAnEmptyGameHoldsOnlyTheGame) {
  startWith(shippedWaves());

  const GamesCensus census = censusOf(probe().runningGame()->game());

  EXPECT_EQ(census.hosted, GAMES_OF_THE_PROCESS);
  EXPECT_EQ(census.inProgress, 0U);
  EXPECT_EQ(census.players, 0U);
  EXPECT_EQ(census.entities, 0U);
}

/**
 * Given three players in the game, nobody ready
 * When the game is counted
 * Then the three players and their three ships are counted, and the game is
 * not in progress
 */
TEST_F(ServerMetricsFixture, TheCensusAddsUpThePlayersAndTheShipsOfTheGame) {
  startWith(shippedWaves());
  join(0);
  join(1);
  join(2);

  const GamesCensus census = censusOf(probe().runningGame()->game());

  EXPECT_EQ(census.inProgress, 0U);
  EXPECT_EQ(census.players, 3U);
  EXPECT_EQ(census.entities, 3U);
}

/**
 * Given a game whose only player is ready, while its client still loads
 * When the game is counted
 * Then it is not in progress: its systems do not run yet
 */
TEST_F(ServerMetricsFixture, TheCensusLeavesOutAGameStillStarting) {
  startWith(shippedWaves());
  join(0);
  sayReady(0);

  EXPECT_EQ(censusOf(probe().runningGame()->game()).inProgress, 0U);
}

/**
 * Given a game started by its only player
 * When the game is counted
 * Then it is in progress
 */
TEST_F(ServerMetricsFixture, TheCensusCountsTheGameInProgress) {
  startWith(shippedWaves());
  join(0);
  startTheGameOfPlayer(0);

  EXPECT_EQ(censusOf(probe().runningGame()->game()).inProgress, 1U);
}

/**
 * Given a game in progress with two timed systems of one name
 * When a tick runs
 * Then that name is listed once, with the time of the two systems
 */
TEST_F(ServerMetricsFixture, AddsUpTheTimeOfTheSystemsOfOneName) {
  startWith(shippedWaves());
  GameInstance& game = probe().runningGame()->game();
  join(0);
  startTheGameOfPlayer(0);
  addTimedSystemTo(game);
  addTimedSystemTo(game);

  runTick({});

  const std::vector<rtype::engine::SystemTime> times = takeSystemTimesOf(game);
  ASSERT_EQ(std::ranges::count(times, TIMED_SYSTEM_NAME,
                               &rtype::engine::SystemTime::name),
            1);
  const auto timed = std::ranges::find(times, TIMED_SYSTEM_NAME,
                                       &rtype::engine::SystemTime::name);
  ASSERT_NE(timed, times.end());
  EXPECT_EQ(timed->total, TIMED_SYSTEM_TIME_OF_TWO_SYSTEMS);
}

/**
 * Given a player of the game measured at a round trip of 42 ms
 * When the round trips of the game are gathered
 * Then that player is listed with the number of the game, 1, its number, its
 * nickname and its figures
 */
TEST_F(ServerMetricsFixture, ListsTheRoundTripOfEveryPlayerMeasured) {
  startWith(shippedWaves());
  ConnectedPlayer& player = join(0);
  player.roundTrip.receive(
      rtype::network::Heartbeat{
          .sentAt = 0,
          .echo = rtype::network::HeartbeatEcho{.sentAt = 0, .heldFor = 0}},
      MEASURED_ROUND_TRIP);

  const std::vector<PlayerRoundTripMeasure> measures =
      roundTripsOf(probe().runningGame()->game());

  ASSERT_EQ(measures.size(), 1U);
  EXPECT_EQ(measures.front().gameNumber, NUMBER_OF_THE_ONLY_GAME);
  EXPECT_EQ(measures.front().playerNumber, player.playerNumber);
  EXPECT_EQ(measures.front().nickname, player.nickname);
  EXPECT_EQ(measures.front().figures,
            (rtype::network::RoundTripFigures{
                .roundTrip = MEASURED_ROUND_TRIP,
                .jitter = std::chrono::microseconds::zero()}));
}

/**
 * Given a game whose only player was measured nothing yet
 * When the round trips of the game are gathered
 * Then nobody is listed
 */
TEST_F(ServerMetricsFixture, LeavesOutAPlayerNotMeasuredYet) {
  startWith(shippedWaves());
  join(0);

  EXPECT_TRUE(roundTripsOf(probe().runningGame()->game()).empty());
}
