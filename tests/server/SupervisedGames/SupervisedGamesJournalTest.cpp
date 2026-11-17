#include <gtest/gtest.h>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>
#include "GameStatus.hpp"
#include "ServerSupervisedGamesFixture.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::network::GameStatusState;

/**
 * Given a journal written as JSON Lines
 * When a player's request starts a game
 * Then one game.started event names the game and the port it listens on
 */
TEST_F(ServerSupervisedGamesFixture, WritesAStartedGameAsAnEvent) {
  writeTheJournalAsJson();
  startSupervisor();

  askForAPlaceAs(0);

  const std::vector<nlohmann::json> events =
      journal().events(STARTED_EVENT_NAME);
  ASSERT_EQ(events.size(), 1U);
  EXPECT_EQ(events.front().at("game").get<std::int64_t>(), FIRST_GAME_NUMBER);
  EXPECT_EQ(std::to_string(events.front().at("port").get<std::int64_t>()),
            argumentAfter(launcher().commands().front(), "-p"));
}

/**
 * Given a journal written as JSON Lines and a game in progress
 * When its process crashes and the supervisor ticks
 * Then one error game.crashed event names the game and its exit code
 */
TEST_F(ServerSupervisedGamesFixture, WritesACrashedGameAsAnErrorEvent) {
  writeTheJournalAsJson();
  startSupervisor();
  askForAPlaceAs(0);
  gameReports(0, statusWithPlayers(GameStatusState::InProgress, 4),
              SEQUENCE_FIVE);

  launcher().exitChild(0, CRASH_EXIT_CODE);
  runTick({});

  const std::vector<nlohmann::json> events =
      journal().events(CRASHED_EVENT_NAME);
  ASSERT_EQ(events.size(), 1U);
  EXPECT_EQ(events.front().at("level").get<std::string>(), ERROR_LEVEL_NAME);
  EXPECT_EQ(events.front().at("game").get<std::int64_t>(), FIRST_GAME_NUMBER);
  EXPECT_EQ(events.front().at("exit_code").get<int>(), CRASH_EXIT_CODE);
  EXPECT_TRUE(journal().events(ENDED_EVENT_NAME).empty());
}

/**
 * Given a journal written as JSON Lines and a game that reported
 * When its process ends by itself with code 0 and the supervisor ticks
 * Then one info game.ended event names the game, without an exit code
 */
TEST_F(ServerSupervisedGamesFixture, WritesAnEndedGameAsAnEvent) {
  writeTheJournalAsJson();
  startSupervisor();
  askForAPlaceAs(0);
  gameReports(0, statusWithPlayers(GameStatusState::InProgress, 4),
              SEQUENCE_FIVE);

  launcher().exitChild(0, 0);
  runTick({});

  const std::vector<nlohmann::json> events = journal().events(ENDED_EVENT_NAME);
  ASSERT_EQ(events.size(), 1U);
  EXPECT_EQ(events.front().at("level").get<std::string>(), INFO_LEVEL_NAME);
  EXPECT_EQ(events.front().at("game").get<std::int64_t>(), FIRST_GAME_NUMBER);
  EXPECT_FALSE(events.front().contains("exit_code"));
}
