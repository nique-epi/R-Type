#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "GameStatisticsFixtures.hpp"
#include "LaunchOptions.hpp"
#include "ModuleConstants.hpp"
#include "ServerConstants.hpp"
#include "StopRequest.hpp"
#include "SupervisorModules.hpp"
#include "SupervisorModulesFixture.hpp"
#include "SupervisorModulesTestConstants.hpp"

using rtype::server::LaunchOptions;
using rtype::server::STATE_RATE_OPTION;
using rtype::server::StopRequest;
using rtype::server::supervisorModules;

/**
 * Given the launch options of a supervisor with no argument
 * When its modules are listed
 * Then they are the journal, the network, the supervised games and the lobby
 * that sends the players to them, in the order they start
 */
TEST(SupervisorModules, ListsTheModulesOfTheSupervisor) {
  StopRequest stopRequest;
  const LaunchOptions options =
      LaunchOptions::read(std::vector<std::string_view>{});

  const std::vector<std::string> gameLines;
  const auto modules = supervisorModules(options, stopRequest, gameLines);

  std::vector<std::string> names;
  names.reserve(modules.size());
  for (const auto& listed : modules) {
    names.push_back(listed->name());
  }
  EXPECT_EQ(names,
            (std::vector<std::string>{
                std::string{rtype::server::LOGGER_MODULE_NAME},
                std::string{rtype::server::NETWORK_MODULE_NAME},
                std::string{rtype::server::SUPERVISED_GAMES_MODULE_NAME},
                std::string{rtype::server::SUPERVISOR_LOBBY_MODULE_NAME}}));
}

/**
 * Given a server.cfg that loads the game-statistics plugin, which needs the
 * games module the supervisor does not have
 * When the supervisor starts
 * Then it starts, refuses no line, and keeps both plugin lines for its games
 */
TEST_F(SupervisorModulesFixture, LeavesThePluginLinesToItsGames) {
  writeConfiguration(gameStatisticsPluginLines());
  const auto application = supervisor({});

  EXPECT_NO_THROW(application->start());

  EXPECT_TRUE(report().refusals.empty());
  EXPECT_EQ(report().handedOverLines, gameStatisticsPluginLines());
  application->stop();
}

/**
 * Given a server.cfg with a plugin, a module of a game turned off and a seed,
 * and a state rate given at launch
 * When the supervisor starts
 * Then the file of the games holds those four lines, the lines of the file
 * first, in their order, and nothing else
 */
TEST_F(SupervisorModulesFixture, WritesTheGameLinesForItsGames) {
  std::vector<std::string> lines = gameStatisticsPluginLines();
  lines.emplace_back(LOBBY_OFF_LINE);
  lines.emplace_back(SEED_SEVEN_LINE);
  writeConfiguration(lines);
  const auto application = supervisor({STATE_RATE_OPTION, STATE_RATE_THIRTY});

  application->start();

  std::string expected;
  for (const std::string& line : lines) {
    expected += line + '\n';
  }
  expected += std::string{STATE_RATE_THIRTY_LINE} + '\n';
  EXPECT_EQ(gameConfigurationText(), expected);
  application->stop();
}

/**
 * Given a server.cfg with a line no program knows
 * When the supervisor starts
 * Then it refuses that line once and hands nothing to its games
 */
TEST_F(SupervisorModulesFixture, RefusesALineThatBelongsToNobody) {
  writeConfiguration({std::string{UNKNOWN_SETTING_LINE}});
  const auto application = supervisor({});

  application->start();

  EXPECT_EQ(report().refusals.size(), 1U);
  EXPECT_TRUE(report().handedOverLines.empty());
  application->stop();
}
