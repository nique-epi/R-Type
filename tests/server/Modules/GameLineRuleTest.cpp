#include <gtest/gtest.h>
#include <string_view>
#include "AdminConsoleTestConstants.hpp"
#include "CommandLine.hpp"
#include "GameLineRule.hpp"
#include "GameLineRuleTestConstants.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerRoleConstants.hpp"

using rtype::engine::splitCommandLine;
using rtype::server::gameLineRule;
using rtype::server::SUPERVISOR_MODULE_NAMES;

/**
 * Given the rule built from the names of the modules of a supervisor
 * When it reads the lines of a setting of the games, a plugin line and a
 * module line on a module the supervisor does not have
 * Then each of them belongs to the games
 */
TEST(GameLineRule, HandsTheLinesOfTheGamesOver) {
  const auto rule = gameLineRule(SUPERVISOR_MODULE_NAMES);

  for (const std::string_view line :
       {SET_SEED_LINE, SET_STATISTICS_FILE_LINE, PLUGIN_LINE,
        PLUGIN_FOLDER_LINE, LOBBY_OFF_RULE_LINE}) {
    SCOPED_TRACE(line);
    EXPECT_TRUE(rule(splitCommandLine(line)));
  }
}

/**
 * Given the rule built from the names of the modules of a supervisor
 * When it reads a module line on a module of the supervisor, a setting of the
 * supervisor, an unknown setting, a module line without a name, the line
 * that turns the console off and a command of the console
 * Then none of them belongs to the games: the supervisor runs or refuses them
 */
TEST(GameLineRule, KeepsTheOtherLinesForTheSupervisor) {
  const auto rule = gameLineRule(SUPERVISOR_MODULE_NAMES);

  for (const std::string_view line :
       {NETWORK_OFF_LINE, SET_PORT_LINE, SET_UNKNOWN_LINE, BARE_MODULE_LINE,
        CONSOLE_OFF_LINE, GAMES_LINE}) {
    SCOPED_TRACE(line);
    EXPECT_FALSE(rule(splitCommandLine(line)));
  }
}
