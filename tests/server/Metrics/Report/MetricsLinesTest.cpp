#include <gtest/gtest.h>
#include <string>
#include "GamesCensus.hpp"
#include "MetricsLines.hpp"
#include "MetricsLinesTestConstants.hpp"
#include "MetricsReport.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "SystemTime.hpp"
#include "TickDurationSummary.hpp"

namespace rtype::server {

using testing::AVERAGE_TICK;
using testing::ENTITIES_ALIVE;
using testing::EXPECTED_LINE_WITHOUT_ROUND_TRIPS;
using testing::EXPECTED_LINE_WITHOUT_TICKS;
using testing::EXPECTED_ROUND_TRIPS_LINE;
using testing::EXPECTED_SYSTEMS_LINE;
using testing::EXPECTED_TICKS_LINE;
using testing::FASTER_SYSTEM_NAME;
using testing::FASTER_SYSTEM_TOTAL;
using testing::FIRST_MEASURED_GAME;
using testing::FIRST_MEASURED_JITTER;
using testing::FIRST_MEASURED_NICKNAME;
using testing::FIRST_MEASURED_ROUND_TRIP;
using testing::GAMES_HOSTED;
using testing::GAMES_IN_PROGRESS;
using testing::MEASURED_PLAYER_NUMBER;
using testing::MEDIAN_TICK;
using testing::NINETY_FIFTH_PERCENTILE_TICK;
using testing::PLAYERS_CONNECTED;
using testing::SECOND_MEASURED_GAME;
using testing::SECOND_MEASURED_JITTER;
using testing::SECOND_MEASURED_NICKNAME;
using testing::SECOND_MEASURED_ROUND_TRIP;
using testing::SLOWER_SYSTEM_NAME;
using testing::SLOWER_SYSTEM_TOTAL;
using testing::TICKS_OF_THE_REPORT;
using testing::WORST_TICK;

/**
 * Given a report: 600 ticks, 2 of 8 games in progress, 5 players, 143 entities
 * When its line about the ticks is written
 * Then it gives each figure, and the budget of a tick to compare them with
 */
TEST(MetricsLines, TheTicksLineGivesEachFigureAndTheBudgetOfATick) {
  const MetricsReport report{
      .ticks = TickDurationSummary{.count = TICKS_OF_THE_REPORT,
                                   .average = AVERAGE_TICK,
                                   .median = MEDIAN_TICK,
                                   .ninetyFifthPercentile =
                                       NINETY_FIFTH_PERCENTILE_TICK,
                                   .worst = WORST_TICK},
      .games = GamesCensus{.hosted = GAMES_HOSTED,
                           .inProgress = GAMES_IN_PROGRESS,
                           .players = PLAYERS_CONNECTED,
                           .entities = ENTITIES_ALIVE},
      .systemTimes = {},
      .traffic = {},
      .roundTrips = {}};

  EXPECT_EQ(ticksLineOf(report), EXPECTED_TICKS_LINE);
}

/**
 * Given a report of 600 ticks and the total time of two systems over them
 * When its line about the systems is written
 * Then it gives what each system costs a tick on average
 */
TEST(MetricsLines, TheSystemsLineGivesTheAverageCostOfEachSystem) {
  const MetricsReport report{
      .ticks = TickDurationSummary{.count = TICKS_OF_THE_REPORT},
      .games = GamesCensus{},
      .systemTimes = {engine::SystemTime{.name = FASTER_SYSTEM_NAME,
                                         .total = FASTER_SYSTEM_TOTAL},
                      engine::SystemTime{.name = SLOWER_SYSTEM_NAME,
                                         .total = SLOWER_SYSTEM_TOTAL}},
      .traffic = {},
      .roundTrips = {}};

  EXPECT_EQ(systemsLineOf(report), EXPECTED_SYSTEMS_LINE);
}

/**
 * Given a report that covers no tick
 * When its line about the systems is written
 * Then each system costs 0 ms, with no division by zero
 */
TEST(MetricsLines, TheSystemsLineOfNoTickGivesZero) {
  const MetricsReport report{
      .ticks = TickDurationSummary{},
      .games = GamesCensus{},
      .systemTimes = {engine::SystemTime{.name = FASTER_SYSTEM_NAME,
                                         .total = FASTER_SYSTEM_TOTAL}},
      .traffic = {},
      .roundTrips = {}};

  EXPECT_EQ(systemsLineOf(report), EXPECTED_LINE_WITHOUT_TICKS);
}

/**
 * Given a report holding the round trip of ACE, player 1 of game 1, and of
 * BOB, player 1 of game 2
 * When its line about the round trips is written
 * Then it gives the game, the number, the nickname, the round trip and the
 * jitter of each, in milliseconds
 */
TEST(MetricsLines, TheRoundTripsLineGivesEachPlayerMeasured) {
  const MetricsReport report{
      .ticks = TickDurationSummary{},
      .games = GamesCensus{},
      .systemTimes = {},
      .traffic = {},
      .roundTrips = {PlayerRoundTripMeasure{
                         .gameNumber = FIRST_MEASURED_GAME,
                         .playerNumber = MEASURED_PLAYER_NUMBER,
                         .nickname = std::string{FIRST_MEASURED_NICKNAME},
                         .figures = {.roundTrip = FIRST_MEASURED_ROUND_TRIP,
                                     .jitter = FIRST_MEASURED_JITTER}},
                     PlayerRoundTripMeasure{
                         .gameNumber = SECOND_MEASURED_GAME,
                         .playerNumber = MEASURED_PLAYER_NUMBER,
                         .nickname = std::string{SECOND_MEASURED_NICKNAME},
                         .figures = {.roundTrip = SECOND_MEASURED_ROUND_TRIP,
                                     .jitter = SECOND_MEASURED_JITTER}}}};

  EXPECT_EQ(roundTripsLineOf(report), EXPECTED_ROUND_TRIPS_LINE);
}

/**
 * Given a report that measured no player
 * When its line about the round trips is written
 * Then it says so
 */
TEST(MetricsLines, TheRoundTripsLineSaysWhenNoPlayerWasMeasured) {
  const MetricsReport report{.ticks = TickDurationSummary{},
                             .games = GamesCensus{},
                             .systemTimes = {},
                             .traffic = {},
                             .roundTrips = {}};

  EXPECT_EQ(roundTripsLineOf(report), EXPECTED_LINE_WITHOUT_ROUND_TRIPS);
}

}  // namespace rtype::server
