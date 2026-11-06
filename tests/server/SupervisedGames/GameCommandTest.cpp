#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <vector>
#include "CommandInterpreter.hpp"
#include "ConfigurationRegistry.hpp"
#include "GameCommand.hpp"
#include "GameLaunch.hpp"
#include "GameLaunchSettings.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "ServerConstants.hpp"
#include "SettingValue.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"
#include "TextSettingDefinition.hpp"

using rtype::server::gameCommand;
using rtype::server::GameLaunch;
using rtype::server::GameLaunchSettings;

/**
 * Given the settings of a supervisor on port 4242 and the launch of game 3 on
 * port 50123 under the token 99
 * When the command of the game is built
 * Then it starts the program as a game process, with its port, its supervisor,
 * its token, the leaderboard of the supervisor's port, its own journal file and
 * the file of commands the supervisor wrote for its games
 */
TEST(GameCommand, GivesTheGameItsPortItsSupervisorItsTokenAndItsFiles) {
  const auto command = gameCommand(
      GameLaunchSettings{.program = PROGRAM_FOR_TESTS,
                         .folder = FOLDER_FOR_TESTS},
      GameLaunch{
          .number = 3, .port = 50123, .supervisorPort = 4242, .linkToken = 99});

  const std::filesystem::path folder{FOLDER_FOR_TESTS};
  EXPECT_EQ(command.program, std::filesystem::path{PROGRAM_FOR_TESTS});
  EXPECT_EQ(command.arguments,
            (std::vector<std::string>{
                "--mode", "game", "-p", "50123", "--supervisor", "4242",
                "--link-token", "99", "--database",
                (folder / "r-type_server_4242.db").string(), "--log-file",
                (folder / "r-type_game_3.log").string(), "--configuration",
                (folder / "r-type_games_4242.cfg").string()}));
}

/**
 * Given a supervisor whose configuration sets its database to
 * some/leaderboard.db
 * When the command of a game is built
 * Then the game is told that file, not the file of the port
 */
TEST(GameCommand, GivesTheGameTheDatabaseOfTheSupervisor) {
  rtype::engine::CommandInterpreter commands;
  rtype::engine::ConfigurationRegistry configuration{commands};
  const rtype::engine::SettingValue& database =
      configuration.declareText(rtype::engine::TextSettingDefinition{
          .name = std::string{rtype::server::DATABASE_SETTING_NAME},
          .defaultValue = {},
          .help = {},
          .validate = {}});
  static_cast<void>(commands.execute(SET_DATABASE_LINE));

  const auto command =
      gameCommand(GameLaunchSettings{.program = PROGRAM_FOR_TESTS,
                                     .folder = FOLDER_FOR_TESTS,
                                     .gameLines = nullptr,
                                     .database = &database},
                  GameLaunch{.number = 0,
                             .port = FIRST_GAME_PORT,
                             .supervisorPort = FIRST_SUPERVISED_PLAYER_PORT,
                             .linkToken = FIRST_LINK_TOKEN});

  EXPECT_EQ(argumentAfter(command, rtype::engine::DATABASE_OPTION),
            DATABASE_OF_THE_SUPERVISOR);
}
