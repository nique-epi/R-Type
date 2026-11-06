#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "CommandLine.hpp"
#include "GameLineRule.hpp"
#include "GameLineRuleTestConstants.hpp"
#include "LaunchOptions.hpp"
#include "StopRequest.hpp"
#include "SupervisorModules.hpp"

using rtype::engine::splitCommandLine;
using rtype::server::gameLineRule;
using rtype::server::LaunchOptions;
using rtype::server::StopRequest;
using rtype::server::supervisorModules;

/**
 * Given the rule built from the modules of a supervisor
 * When it reads the lines of a setting of the games, a plugin line and a
 * module line on a module the supervisor does not have
 * Then each of them belongs to the games
 */
TEST(GameLineRule, HandsTheLinesOfTheGamesOver) {
  StopRequest stopRequest;
  const std::vector<std::string> gameLines;
  const auto rule = gameLineRule(
      supervisorModules(LaunchOptions::read(std::vector<std::string_view>{}),
                        stopRequest, gameLines));

  for (const std::string_view line :
       {SET_SEED_LINE, SET_STATISTICS_FILE_LINE, PLUGIN_LINE,
        PLUGIN_FOLDER_LINE, LOBBY_OFF_RULE_LINE}) {
    SCOPED_TRACE(line);
    EXPECT_TRUE(rule(splitCommandLine(line)));
  }
}

/**
 * Given the rule built from the modules of a supervisor
 * When it reads a module line on a module of the supervisor, a setting of the
 * supervisor, an unknown setting and a module line without a name
 * Then none of them belongs to the games: the supervisor runs or refuses them
 */
TEST(GameLineRule, KeepsTheOtherLinesForTheSupervisor) {
  StopRequest stopRequest;
  const std::vector<std::string> gameLines;
  const auto rule = gameLineRule(
      supervisorModules(LaunchOptions::read(std::vector<std::string_view>{}),
                        stopRequest, gameLines));

  for (const std::string_view line :
       {NETWORK_OFF_LINE, SET_PORT_LINE, SET_UNKNOWN_LINE, BARE_MODULE_LINE}) {
    SCOPED_TRACE(line);
    EXPECT_FALSE(rule(splitCommandLine(line)));
  }
}
