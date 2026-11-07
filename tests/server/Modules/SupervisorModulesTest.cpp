#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "BuiltModuleConstants.hpp"
#include "GameStatisticsFixtures.hpp"
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "ModuleConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerInformation.hpp"
#include "ServerRoleConstants.hpp"
#include "StopRequest.hpp"
#include "SupervisorModules.hpp"
#include "SupervisorModulesFixture.hpp"
#include "SupervisorModulesTestConstants.hpp"

using rtype::engine::MASTER_LINK_MODULE_BUILT;
using rtype::server::LaunchOptions;
using rtype::server::STATE_RATE_OPTION;
using rtype::server::StopRequest;
using rtype::server::SUPERVISOR_MODULE_NAMES;
using rtype::server::supervisorModules;

/**
 * Given the launch options of a supervisor with no argument, in a build that
 * compiled the master link
 * When its modules are listed
 * Then they are the journal, the network, the supervised games, the lobby
 * that sends the players to them, then the information and the master, which
 * read what the supervised games publish, in the order they start
 */
TEST(SupervisorModules, ListsTheModulesOfTheSupervisor) {
  if (!MASTER_LINK_MODULE_BUILT) {
    GTEST_SKIP();
  }
  StopRequest stopRequest;
  const LaunchOptions options =
      LaunchOptions::read(std::vector<std::string_view>{});

  const auto modules = supervisorModules(options, stopRequest, {});

  std::vector<std::string> names;
  names.reserve(modules.size());
  for (const auto& listed : modules) {
    names.push_back(listed->name());
  }
  EXPECT_EQ(names, (std::vector<std::string>{
                       std::string{rtype::server::LOGGER_MODULE_NAME},
                       std::string{rtype::server::NETWORK_MODULE_NAME},
                       std::string{rtype::server::SUPERVISED_GAMES_MODULE_NAME},
                       std::string{rtype::server::SUPERVISOR_LOBBY_MODULE_NAME},
                       std::string{rtype::server::INFORMATION_MODULE_NAME},
                       std::string{rtype::server::MASTER_MODULE_NAME}}));
}

/**
 * Given the launch options of a supervisor, in a build with or without the
 * master link
 * When its modules are listed
 * Then the master is among them exactly when the build compiled it
 */
TEST(SupervisorModules, ListsTheMasterLinkOnlyWhenItIsBuilt) {
  StopRequest stopRequest;
  const auto modules = supervisorModules(
      LaunchOptions::read(std::vector<std::string_view>{}), stopRequest, {});

  const bool listed = std::ranges::any_of(
      modules, [](const std::unique_ptr<rtype::engine::IModule>& module) {
        return module->name() == rtype::server::MASTER_MODULE_NAME;
      });

  EXPECT_EQ(listed, MASTER_LINK_MODULE_BUILT);
}

/**
 * Given the modules of a supervisor
 * When their names are compared with the names the rule of the lines of the
 * games is built from, before the modules exist
 * Then they are the same, in the same order: a line on a module of the
 * supervisor is never handed to the games
 */
TEST(SupervisorModules, TheRuleKnowsEveryModuleOfTheSupervisor) {
  StopRequest stopRequest;
  const auto modules = supervisorModules(
      LaunchOptions::read(std::vector<std::string_view>{}), stopRequest, {});

  ASSERT_EQ(modules.size(), SUPERVISOR_MODULE_NAMES.size());
  for (std::size_t index = 0; index < modules.size(); ++index) {
    EXPECT_EQ(modules[index]->name(), SUPERVISOR_MODULE_NAMES.at(index));
  }
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

/**
 * Given a server.cfg that sets server-name and database, settings the
 * supervisor owns
 * When the supervisor starts
 * Then it refuses none of them and hands none to its games
 */
TEST_F(SupervisorModulesFixture, KeepsTheLinesOfTheSettingsItOwns) {
  writeConfiguration(
      {std::string{SERVER_NAME_LINE}, std::string{DATABASE_LINE}});
  const auto application = supervisor({});

  application->start();

  EXPECT_TRUE(report().refusals.empty());
  EXPECT_TRUE(report().handedOverLines.empty());
  application->stop();
}

/**
 * Given a server.cfg that sets master-url and master-key, in a build that
 * compiled the master link, which declares them
 * When the supervisor starts
 * Then it refuses neither and hands neither to its games
 */
TEST_F(SupervisorModulesFixture, KeepsTheLinesOfTheMasterSettings) {
  if (!MASTER_LINK_MODULE_BUILT) {
    GTEST_SKIP();
  }
  writeConfiguration(
      {std::string{MASTER_URL_LINE}, std::string{MASTER_KEY_LINE}});
  const auto application = supervisor({});

  application->start();

  EXPECT_TRUE(report().refusals.empty());
  EXPECT_TRUE(report().handedOverLines.empty());
  application->stop();
}

/**
 * Given a supervisor that runs no game yet, built from its real list of
 * modules
 * When a stranger asks for its information
 * Then it is told no player, the eight games it may run and their 32 seats
 * free
 */
TEST_F(SupervisorModulesFixture, AnswersHowFullAnIdleSupervisorIs) {
  const auto application = supervisor({});
  application->start();

  const rtype::network::ServerInformation answer = askInformation(*application);

  EXPECT_EQ(answer.playerCount, 0U);
  EXPECT_EQ(answer.gameCount, GAMES_OF_A_SUPERVISOR);
  EXPECT_EQ(answer.freeSeatCount, SEATS_OF_A_SUPERVISOR);
  application->stop();
}
