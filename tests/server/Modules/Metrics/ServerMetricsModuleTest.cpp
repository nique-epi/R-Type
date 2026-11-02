#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EntityRegistry.hpp"
#include "GameInstance.hpp"
#include "GamesCensus.hpp"
#include "Instances/GameInstanceMeasuresTestConstants.hpp"
#include "MetricsReport.hpp"
#include "ServerMetricsFixture.hpp"
#include "ServerMetricsTestConstants.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerTestConstants.hpp"
#include "SystemScheduler.hpp"
#include "SystemTime.hpp"
#include "TickDurationSummary.hpp"
#include "TimeConstants.hpp"

using rtype::server::GamesCensus;
using rtype::server::MetricsReport;
using rtype::server::TickDurationSummary;

/**
 * Given a server that ran one tick less than a report
 * When the metrics are read
 * Then no report was published and the journal holds no line of the metrics
 */
TEST_F(ServerMetricsFixture, PublishesNothingBeforeTheEndOfAReport) {
  startWithGames(ONE_GAME);

  runTicks(ONE_TICK_BEFORE_THE_REPORT);

  EXPECT_FALSE(latestReport().has_value());
  EXPECT_EQ(metricsLinesInTheJournal(), 0U);
}

/**
 * Given a server whose ticks each spend TICK_WORK_TIME in the simulation stage
 * When it ran the ticks of ten seconds
 * Then the report counts those ticks and gives that time for each figure
 */
TEST_F(ServerMetricsFixture, ReportsHowLongTheTicksLasted) {
  startWithGames(ONE_GAME);

  runTicks(TICKS_IN_TEN_SECONDS);

  const std::optional<MetricsReport> report = latestReport();
  ASSERT_TRUE(report.has_value());
  const TickDurationSummary ticks = report.value_or(MetricsReport{}).ticks;
  EXPECT_EQ(ticks.count, TICKS_IN_TEN_SECONDS);
  EXPECT_EQ(ticks.average, TICK_WORK_TIME);
  EXPECT_EQ(ticks.median, TICK_WORK_TIME);
  EXPECT_EQ(ticks.ninetyFifthPercentile, TICK_WORK_TIME);
  EXPECT_EQ(ticks.worst, TICK_WORK_TIME);
}

/**
 * Given a server
 * When it ran the ticks of ten seconds
 * Then the journal holds the two lines of one report
 */
TEST_F(ServerMetricsFixture, WritesTwoLinesInTheJournalEveryReport) {
  startWithGames(ONE_GAME);

  runTicks(TICKS_IN_TEN_SECONDS);

  EXPECT_EQ(metricsLinesInTheJournal(), LINES_OF_ONE_REPORT);
}

/**
 * Given a server that published a report
 * When it runs the ticks of another ten seconds
 * Then the second report counts its own ticks only
 */
TEST_F(ServerMetricsFixture, TheNextReportStartsAfresh) {
  startWithGames(ONE_GAME);

  runTicks(TICKS_OF_TWO_REPORTS);

  const std::optional<MetricsReport> report = latestReport();
  ASSERT_TRUE(report.has_value());
  EXPECT_EQ(report.value_or(MetricsReport{}).ticks.count, TICKS_IN_TEN_SECONDS);
  EXPECT_EQ(metricsLinesInTheJournal(), LINES_OF_TWO_REPORTS);
}

/**
 * Given a server of three games, one player seated in the first one
 * When it ran the ticks of ten seconds
 * Then the report counts the games, the player and its ship
 */
TEST_F(ServerMetricsFixture, ReportsTheGamesThePlayersAndTheEntities) {
  startWithGames(THREE_GAMES);
  join(0);

  runTicks(TICKS_IN_TEN_SECONDS);

  const std::optional<MetricsReport> report = latestReport();
  ASSERT_TRUE(report.has_value());
  const GamesCensus games = report.value_or(MetricsReport{}).games;
  EXPECT_EQ(games.hosted, THREE_GAMES);
  EXPECT_EQ(games.inProgress, 0U);
  EXPECT_EQ(games.players, 1U);
  EXPECT_EQ(games.entities, 1U);
}

/**
 * Given a server of three games, each with the same systems
 * When it ran the ticks of ten seconds
 * Then the report lists each system once
 */
TEST_F(ServerMetricsFixture, ReportsEachSystemOnce) {
  startWithGames(THREE_GAMES);

  runTicks(TICKS_IN_TEN_SECONDS);

  const std::optional<MetricsReport> report = latestReport();
  ASSERT_TRUE(report.has_value());
  EXPECT_EQ(report.value_or(MetricsReport{}).systemTimes.size(),
            rtype::server::testing::NAMES_OF_THE_SYSTEMS.size());
}

/**
 * Given a stopped metrics module, and a game with a system of a known time
 * When that system runs
 * Then its time is not counted: the module took its clock back
 */
TEST_F(ServerMetricsFixture, StopsMeasuringTheSystemsWhenItStops) {
  startWithGames(ONE_GAME);
  rtype::server::GameInstance& game = *probe().games()->all()[FIRST_GAME];
  addTimedSystemTo(game);
  application().stop();
  rtype::engine::EntityRegistry entities;
  rtype::engine::ComponentRegistry components(entities);

  game.systems().run(components, rtype::engine::SIMULATION_TICK_DURATION);

  const std::vector<rtype::engine::SystemTime> times =
      game.systems().takeSystemTimes();
  EXPECT_EQ(times.back().total, rtype::engine::Duration::zero());
}
