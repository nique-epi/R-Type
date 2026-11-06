#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "GameLifecycle.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "ServerMeasures.hpp"
#include "ServerOccupancy.hpp"
#include "SupervisedGame.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesReport.hpp"
#include "SupervisedGamesReportTestConstants.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::network::HostedGame;
using rtype::network::HostedGameState;
using rtype::network::ServerMeasures;
using rtype::server::GameLifecycle;
using rtype::server::ServerOccupancy;
using rtype::server::SupervisedGame;
using rtype::server::supervisedHostedGames;
using rtype::server::supervisedMeasures;
using rtype::server::supervisedOccupancy;

/**
 * Given a game in place 1 that was launched and never reported
 * When the master list is written
 * Then it is game 1, named GAME 1, in the normal difficulty, waiting, with no
 * player
 */
TEST(SupervisedGamesReport, ListsAGameThatStartsAsWaiting) {
  std::vector<SupervisedGame> games;
  games.push_back(supervisedGameIn(FIRST_PLACE, GameLifecycle::Starting, 0));

  const std::vector<HostedGame> hosted = supervisedHostedGames(games);

  ASSERT_EQ(hosted.size(), 1U);
  EXPECT_EQ(hosted.front(),
            (HostedGame{.number = FIRST_PLACE,
                        .name = std::string{NAME_OF_THE_GAME_IN_PLACE_1},
                        .state = HostedGameState::Waiting,
                        .difficulty = std::string{DIFFICULTY_OF_EVERY_GAME},
                        .players = {}}));
}

/**
 * Given a game that waits for players and holds four
 * When the master list is written
 * Then it is full, and names its four players
 */
TEST(SupervisedGamesReport, ListsAWaitingGameOfFourAsFull) {
  std::vector<SupervisedGame> games;
  games.push_back(supervisedGameIn(
      FIRST_PLACE, GameLifecycle::WaitingForPlayers, FOUR_PLAYERS));

  const HostedGame hosted = supervisedHostedGames(games).front();

  EXPECT_EQ(hosted.state, HostedGameState::Full);
  EXPECT_EQ(hosted.players.size(), FOUR_PLAYERS);
}

/**
 * Given a game that waits for players and holds three
 * When the master list is written
 * Then it is waiting
 */
TEST(SupervisedGamesReport, ListsAWaitingGameOfThreeAsWaiting) {
  std::vector<SupervisedGame> games;
  games.push_back(supervisedGameIn(
      FIRST_PLACE, GameLifecycle::WaitingForPlayers, THREE_PLAYERS));

  EXPECT_EQ(supervisedHostedGames(games).front().state,
            HostedGameState::Waiting);
}

/**
 * Given a game in progress
 * When the master list is written
 * Then it is playing
 */
TEST(SupervisedGamesReport, ListsAGameInProgressAsPlaying) {
  std::vector<SupervisedGame> games;
  games.push_back(
      supervisedGameIn(FIRST_PLACE, GameLifecycle::InProgress, TWO_PLAYERS));

  EXPECT_EQ(supervisedHostedGames(games).front().state,
            HostedGameState::Playing);
}

/**
 * Given a game whose round finished
 * When the master list is written
 * Then it is finished
 */
TEST(SupervisedGamesReport, ListsAFinishedGameAsFinished) {
  std::vector<SupervisedGame> games;
  games.push_back(supervisedGameIn(FIRST_PLACE, GameLifecycle::Finished, 0));

  EXPECT_EQ(supervisedHostedGames(games).front().state,
            HostedGameState::Finished);
}

/**
 * Given a game in place 2 launched before a game in place 1
 * When the master list is written
 * Then the games are numbered and named by their place, place 1 first
 */
TEST(SupervisedGamesReport, NumbersTheGamesByTheirPlace) {
  std::vector<SupervisedGame> games;
  games.push_back(supervisedGameIn(SECOND_PLACE, GameLifecycle::Starting, 0));
  games.push_back(supervisedGameIn(FIRST_PLACE, GameLifecycle::Starting, 0));

  const std::vector<HostedGame> hosted = supervisedHostedGames(games);

  ASSERT_EQ(hosted.size(), 2U);
  EXPECT_EQ(hosted[0].number, FIRST_PLACE);
  EXPECT_EQ(hosted[0].name, NAME_OF_THE_GAME_IN_PLACE_1);
  EXPECT_EQ(hosted[1].number, SECOND_PLACE);
  EXPECT_EQ(hosted[1].name, NAME_OF_THE_GAME_IN_PLACE_2);
}

/**
 * Given a game ticking at 2 ms and 5 ms, and one at 3 ms and 4 ms
 * When their measures are written
 * Then the median is 3 ms and the 95th percentile 5 ms, the highest of each
 */
TEST(SupervisedGamesReport, TellsTheTicksOfTheSlowestGame) {
  std::vector<SupervisedGame> games;
  games.push_back(
      supervisedGameIn(FIRST_PLACE, GameLifecycle::InProgress, ONE_PLAYER));
  games.back().measures.tickMedianMicroseconds = FIRST_GAME_MEDIAN_MICROSECONDS;
  games.back().measures.tickNinetyFifthPercentileMicroseconds =
      FIRST_GAME_P95_MICROSECONDS;
  games.push_back(
      supervisedGameIn(SECOND_PLACE, GameLifecycle::InProgress, ONE_PLAYER));
  games.back().measures.tickMedianMicroseconds =
      SECOND_GAME_MEDIAN_MICROSECONDS;
  games.back().measures.tickNinetyFifthPercentileMicroseconds =
      SECOND_GAME_P95_MICROSECONDS;

  const ServerMeasures measures = supervisedMeasures(games);

  EXPECT_DOUBLE_EQ(measures.tickP50Milliseconds.value_or(0.0),
                   SLOWEST_MEDIAN_MILLISECONDS);
  EXPECT_DOUBLE_EQ(measures.tickP95Milliseconds.value_or(0.0),
                   SLOWEST_P95_MILLISECONDS);
}

/**
 * Given a game of one player 10 ms away and a game of three players 30 ms
 * away
 * When their measures are written
 * Then the mean round trip is 25 ms, each game counting once per player
 */
TEST(SupervisedGamesReport, WeighsTheRoundTripOfEachGameByItsPlayers) {
  std::vector<SupervisedGame> games;
  games.push_back(
      supervisedGameIn(FIRST_PLACE, GameLifecycle::InProgress, ONE_PLAYER));
  games.back().measures.averageRoundTripMicroseconds =
      NEAR_GAME_ROUND_TRIP_MICROSECONDS;
  games.push_back(
      supervisedGameIn(SECOND_PLACE, GameLifecycle::InProgress, THREE_PLAYERS));
  games.back().measures.averageRoundTripMicroseconds =
      FAR_GAME_ROUND_TRIP_MICROSECONDS;

  EXPECT_DOUBLE_EQ(
      supervisedMeasures(games).averagePingMilliseconds.value_or(0.0),
      WEIGHTED_ROUND_TRIP_MILLISECONDS);
}

/**
 * Given a game that reported no measure
 * When the measures are written
 * Then each is left out, never sent as 0
 */
TEST(SupervisedGamesReport, LeavesOutAMeasureNoGameReports) {
  std::vector<SupervisedGame> games;
  games.push_back(
      supervisedGameIn(FIRST_PLACE, GameLifecycle::InProgress, TWO_PLAYERS));

  EXPECT_EQ(supervisedMeasures(games), ServerMeasures{});
}

/**
 * Given a supervisor that runs no game
 * When its occupancy is counted
 * Then it has no player, eight games and their 32 seats free
 */
TEST(SupervisedGamesReport, CountsEverySeatOfAnIdleSupervisorAsFree) {
  const ServerOccupancy occupancy =
      supervisedOccupancy(std::vector<SupervisedGame>{});

  EXPECT_EQ(occupancy.playerCount, 0U);
  EXPECT_EQ(occupancy.gameCount, GAMES_A_SUPERVISOR_RUNS);
  EXPECT_EQ(occupancy.freeSeatCount, SEATS_OF_AN_IDLE_SUPERVISOR);
}

/**
 * Given one game that waits for players and holds one
 * When the occupancy is counted
 * Then it has one player and 31 seats free
 */
TEST(SupervisedGamesReport, CountsTheEmptySeatsOfAWaitingGame) {
  std::vector<SupervisedGame> games;
  games.push_back(supervisedGameIn(
      FIRST_PLACE, GameLifecycle::WaitingForPlayers, ONE_PLAYER));

  const ServerOccupancy occupancy = supervisedOccupancy(games);

  EXPECT_EQ(occupancy.playerCount, ONE_PLAYER);
  EXPECT_EQ(occupancy.freeSeatCount, SEATS_BESIDE_ONE_WAITING_PLAYER);
}

/**
 * Given one game of two players in progress
 * When the occupancy is counted
 * Then it has two players, and only the seats of the seven other places are
 * free: a game in progress gives no seat
 */
TEST(SupervisedGamesReport, CountsNoFreeSeatInAGameInProgress) {
  std::vector<SupervisedGame> games;
  games.push_back(
      supervisedGameIn(FIRST_PLACE, GameLifecycle::InProgress, TWO_PLAYERS));

  const ServerOccupancy occupancy = supervisedOccupancy(games);

  EXPECT_EQ(occupancy.playerCount, TWO_PLAYERS);
  EXPECT_EQ(occupancy.freeSeatCount, SEATS_OF_SEVEN_FREE_PLACES);
}
