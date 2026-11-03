#include <gtest/gtest.h>
#include "GamesCensus.hpp"
#include "MetricsLines.hpp"
#include "MetricsLinesTestConstants.hpp"
#include "MetricsReport.hpp"
#include "SystemTime.hpp"
#include "TickDurationSummary.hpp"

namespace rtype::server {

using testing::AVERAGE_TICK;
using testing::ENTITIES_ALIVE;
using testing::EXPECTED_LINE_WITHOUT_TICKS;
using testing::EXPECTED_SYSTEMS_LINE;
using testing::EXPECTED_TICKS_LINE;
using testing::FASTER_SYSTEM_NAME;
using testing::FASTER_SYSTEM_TOTAL;
using testing::GAMES_HOSTED;
using testing::GAMES_IN_PROGRESS;
using testing::MEDIAN_TICK;
using testing::NINETY_FIFTH_PERCENTILE_TICK;
using testing::PLAYERS_CONNECTED;
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
      .traffic = {}};

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
      .traffic = {}};

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
      .traffic = {}};

  EXPECT_EQ(systemsLineOf(report), EXPECTED_LINE_WITHOUT_TICKS);
}

}  // namespace rtype::server
