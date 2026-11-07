#include <gtest/gtest.h>
#include <algorithm>
#include <iterator>
#include <set>
#include <string>
#include "ModuleConstants.hpp"
#include "ServerRoleConstants.hpp"
#include "ServerRolesFixture.hpp"
#include "ServerRolesTestConstants.hpp"

using rtype::server::GAME_SETTING_NAMES;

/**
 * Given server.cfg.example, which shows some settings only commented out
 * When its `set` lines are read
 * Then the commented ones are read too, so no setting of the file is skipped
 */
TEST_F(ServerRolesFixture, ReadsTheCommentedSettingsOfTheExampleToo) {
  const std::set<std::string> example = exampleSettings();

  EXPECT_TRUE(example.contains(std::string{COMMENTED_EXAMPLE_SETTING}));
}

/**
 * Given the settings of server.cfg.example, and the modules of a supervisor
 * and of a game process, its plugin loaded
 * When each setting the catalogue gives the games is looked for
 * Then a game declares it and the supervisor does not, so the line the
 * supervisor hands over is run where it means something
 */
TEST_F(ServerRolesFixture, ASettingOfTheGamesIsDeclaredByTheGamesOnly) {
  const std::set<std::string> supervisor = supervisorSettings();
  const std::set<std::string> game = gameSettings();

  for (const std::string& name : exampleSettingsOfThisBuild()) {
    if (std::ranges::find(GAME_SETTING_NAMES, name) !=
        GAME_SETTING_NAMES.end()) {
      SCOPED_TRACE(name);
      EXPECT_TRUE(game.contains(name));
      EXPECT_FALSE(supervisor.contains(name));
    }
  }
}

/**
 * Given the settings of server.cfg.example of the modules this build holds,
 * and the modules of a supervisor
 * When each setting the catalogue does not give the games is looked for
 * Then the supervisor declares it, so the line it keeps is never refused
 */
TEST_F(ServerRolesFixture, AnyOtherSettingOfTheExampleIsTheSupervisors) {
  const std::set<std::string> supervisor = supervisorSettings();

  for (const std::string& name : exampleSettingsOfThisBuild()) {
    if (std::ranges::find(GAME_SETTING_NAMES, name) ==
        GAME_SETTING_NAMES.end()) {
      SCOPED_TRACE(name);
      EXPECT_TRUE(supervisor.contains(name));
    }
  }
}

/**
 * Given the modules of a game process, its plugin loaded
 * When the settings they declare are listed
 * Then each one is either named by the catalogue, so its line reaches the
 * games, or one the supervisor gives the game on its command line
 */
TEST_F(ServerRolesFixture, AGameDeclaresOnlyWhatReachesItsProcess) {
  for (const std::string& name : gameSettings()) {
    SCOPED_TRACE(name);
    EXPECT_TRUE(std::ranges::find(GAME_SETTING_NAMES, name) !=
                    GAME_SETTING_NAMES.end() ||
                std::ranges::find(SETTINGS_A_GAME_IS_LAUNCHED_WITH, name) !=
                    SETTINGS_A_GAME_IS_LAUNCHED_WITH.end());
  }
}

/**
 * Given the modules of a supervisor and of a game process
 * When the settings both declare are listed
 * Then they are only the port and the database, which the supervisor gives
 * each game on its command line
 */
TEST_F(ServerRolesFixture, TheRolesShareOnlyWhatAGameIsLaunchedWith) {
  const std::set<std::string> supervisor = supervisorSettings();
  const std::set<std::string> game = gameSettings();

  std::set<std::string> shared;
  std::ranges::set_intersection(supervisor, game,
                                std::inserter(shared, shared.end()));

  EXPECT_EQ(shared,
            (std::set<std::string>{SETTINGS_A_GAME_IS_LAUNCHED_WITH.begin(),
                                   SETTINGS_A_GAME_IS_LAUNCHED_WITH.end()}));
}

/**
 * Given the modules of a supervisor and the catalogue of the games
 * When their settings are looked for in server.cfg.example
 * Then each one is there, so the file shows every setting server.cfg takes
 */
TEST_F(ServerRolesFixture, TheExampleShowsEverySettingOfServerConfiguration) {
  const std::set<std::string> example = exampleSettings();
  std::set<std::string> read = supervisorSettings();
  read.insert(GAME_SETTING_NAMES.begin(), GAME_SETTING_NAMES.end());

  for (const std::string& name : read) {
    SCOPED_TRACE(name);
    EXPECT_TRUE(example.contains(name));
  }
}

/**
 * Given the modules of a game process
 * When their names are read
 * Then neither the information nor the master is among them: only the
 * supervisor answers the players and tells the master
 */
TEST_F(ServerRolesFixture, AGameHoldsNeitherTheInformationNorTheMaster) {
  for (const auto& module : gameModules()) {
    SCOPED_TRACE(module->name());
    EXPECT_NE(module->name(), rtype::server::INFORMATION_MODULE_NAME);
    EXPECT_NE(module->name(), rtype::server::MASTER_MODULE_NAME);
  }
}
