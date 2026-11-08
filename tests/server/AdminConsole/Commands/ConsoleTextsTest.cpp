#include <gtest/gtest.h>
#include <chrono>
#include <optional>
#include <string>
#include <vector>
#include "AdminConsoleTestConstants.hpp"
#include "ConsoleTexts.hpp"
#include "ConsoleTextsTestConstants.hpp"
#include "GameLifecycle.hpp"
#include "GameStatusMeasures.hpp"
#include "ServerMeasures.hpp"
#include "SupervisedGame.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesReportTestConstants.hpp"
#include "SupervisedGamesTestConstants.hpp"
#include "TrafficTotals.hpp"

using rtype::network::GameStatusMeasures;
using rtype::network::ServerMeasures;
using rtype::network::TrafficTotals;
using rtype::server::GameLifecycle;
using rtype::server::gameLinesOf;
using rtype::server::lengthText;
using rtype::server::measuresLineOf;
using rtype::server::networkLinesOf;
using rtype::server::playerLinesOf;
using rtype::server::SupervisedGame;

/**
 * Given a game in place 1 whose status gave its tick and its round trip
 * When the lines of the games are written
 * Then its line names its place, number, state, pilots, port and both
 * measures in milliseconds
 */
TEST(ConsoleTexts, WritesOneLinePerGameWithItsMeasures) {
  std::vector<SupervisedGame> games;
  games.push_back(
      supervisedGameIn(FIRST_PLACE, GameLifecycle::InProgress, TWO_PLAYERS));
  games.front().measures = GameStatusMeasures{
      .tickMedianMicroseconds = std::nullopt,
      .tickNinetyFifthPercentileMicroseconds = TICK_P95_OF_800_MICROSECONDS,
      .averageRoundTripMicroseconds = ROUND_TRIP_OF_24000_MICROSECONDS};

  EXPECT_EQ(gameLinesOf(games),
            (std::vector<std::string>{std::string{FIRST_GAME_MEASURED_LINE}}));
}

/**
 * Given a game whose status gave no measure
 * When the lines of the games are written
 * Then its line ends at its port
 */
TEST(ConsoleTexts, LeavesOutTheMeasuresNotTaken) {
  std::vector<SupervisedGame> games;
  games.push_back(
      supervisedGameIn(FIRST_PLACE, GameLifecycle::InProgress, TWO_PLAYERS));

  EXPECT_EQ(
      gameLinesOf(games),
      (std::vector<std::string>{std::string{FIRST_GAME_UNMEASURED_LINE}}));
}

/**
 * Given no game
 * When the lines of the games and of the pilots are written
 * Then each is the one line that says no game runs
 */
TEST(ConsoleTexts, SaysWhenNoGameRuns) {
  const std::vector<SupervisedGame> games;

  EXPECT_EQ(gameLinesOf(games),
            (std::vector<std::string>{std::string{NO_GAME_LINE}}));
  EXPECT_EQ(playerLinesOf(games),
            (std::vector<std::string>{std::string{NO_GAME_LINE}}));
}

/**
 * Given a game with two pilots, then a game with none
 * When the lines of the pilots are written
 * Then the first names its pilots, the second says it has none
 */
TEST(ConsoleTexts, ListsThePilotsOfEachGame) {
  std::vector<SupervisedGame> games;
  games.push_back(
      supervisedGameIn(FIRST_PLACE, GameLifecycle::InProgress, TWO_PLAYERS));
  games.push_back(
      supervisedGameIn(SECOND_PLACE, GameLifecycle::WaitingForPlayers, 0));

  EXPECT_EQ(playerLinesOf(games),
            (std::vector<std::string>{std::string{FIRST_GAME_PILOTS_LINE},
                                      std::string{SECOND_GAME_NO_PILOT_LINE}}));
}

/**
 * Given measures of the games holding a median tick and a loss only
 * When their line is written
 * Then it names those two, and no other
 */
TEST(ConsoleTexts, WritesEachMeasureTakenOfTheGames) {
  const ServerMeasures measures{
      .tickP50Milliseconds = MEDIAN_TICK_OF_HALF_A_MILLISECOND,
      .tickP95Milliseconds = std::nullopt,
      .receivedBytesPerSecond = std::nullopt,
      .sentBytesPerSecond = std::nullopt,
      .lossPercent = LOSS_OF_TWO_PERCENT,
      .averagePingMilliseconds = std::nullopt};

  EXPECT_EQ(measuresLineOf(measures), TWO_MEASURES_LINE);
}

/**
 * Given no measure of the games yet
 * When their line is written
 * Then it says so
 */
TEST(ConsoleTexts, SaysWhenTheGamesWereNotMeasuredYet) {
  EXPECT_EQ(measuresLineOf(std::nullopt), NO_MEASURE_LINE);
}

/**
 * Given measures of the games in which no measure was taken, as an idle
 * supervisor publishes them
 * When their line is written
 * Then it says the games were not measured yet
 */
TEST(ConsoleTexts, SaysWhenNoMeasureOfTheGamesWasTaken) {
  EXPECT_EQ(measuresLineOf(ServerMeasures{}), NO_MEASURE_LINE);
}

/**
 * Given seven datagrams rejected since the start
 * When the lines of the network are written
 * Then the second one counts them
 */
TEST(ConsoleTexts, CountsTheRejectedDatagrams) {
  const std::vector<std::string> lines =
      networkLinesOf(TrafficTotals{}, std::chrono::seconds{1}, SEVEN_REJECTED);

  ASSERT_EQ(lines.size(), 2U);
  EXPECT_EQ(lines[1], SEVEN_REJECTED_LINE);
}

/**
 * Given a length of thirty minutes, of two hours and of seven days
 * When each is written
 * Then each names its one unit
 */
TEST(ConsoleTexts, WritesALengthOfOneUnit) {
  EXPECT_EQ(lengthText(THIRTY_MINUTES_LENGTH), THIRTY_MINUTES_TEXT);
  EXPECT_EQ(lengthText(TWO_HOURS_LENGTH), TWO_HOURS_TEXT);
  EXPECT_EQ(lengthText(SEVEN_DAYS_LENGTH), SEVEN_DAYS_TEXT);
}

/**
 * Given a day and a minute, and 1 h 58 min 1 s
 * When each is written
 * Then the units that are not zero are named, and the part of a minute counts
 * as a whole minute
 */
TEST(ConsoleTexts, WritesTheUnitsThatAreNotZeroRoundedUpToTheMinute) {
  EXPECT_EQ(lengthText(A_DAY_AND_A_MINUTE_LENGTH), A_DAY_AND_A_MINUTE_TEXT);
  EXPECT_EQ(lengthText(ALMOST_TWO_HOURS_LENGTH), ALMOST_TWO_HOURS_TEXT);
}
