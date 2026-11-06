#include <gtest/gtest.h>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include "CommandInterpreter.hpp"
#include "ConfigurationRegistry.hpp"
#include "GameCommand.hpp"
#include "GameLaunch.hpp"
#include "GameLaunchOptions.hpp"
#include "GameLaunchSettings.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOptions.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "SettingValue.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"
#include "TextSettingDefinition.hpp"

using rtype::server::gameCommand;
using rtype::server::GameLaunch;
using rtype::server::GameLaunchOptions;
using rtype::server::GameLaunchSettings;
using rtype::server::LaunchOptions;

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
                         .folder = FOLDER_FOR_TESTS,
                         .forwardedArguments = {}},
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
                                     .database = &database,
                                     .forwardedArguments = {}},
                  GameLaunch{.number = 0,
                             .port = FIRST_GAME_PORT,
                             .supervisorPort = FIRST_SUPERVISED_PLAYER_PORT,
                             .linkToken = FIRST_LINK_TOKEN});

  EXPECT_EQ(argumentAfter(command, rtype::engine::DATABASE_OPTION),
            DATABASE_OF_THE_SUPERVISOR);
}

/**
 * Given a supervisor given `--traffic-csv some/folder/traffic.csv`
 * When the command of game 3 is built
 * Then the game writes its traffic to some/folder/traffic_game_3.csv, a file
 * of its own
 */
TEST(GameCommand, GivesEachGameATrafficFileOfItsOwn) {
  const auto command = gameCommand(
      GameLaunchSettings{
          .program = PROGRAM_FOR_TESTS,
          .folder = FOLDER_FOR_TESTS,
          .forwardedArguments = {},
          .trafficCsvFile =
              std::filesystem::path{TRAFFIC_FILE_OF_THE_SUPERVISOR}},
      GameLaunch{.number = GAME_THREE,
                 .port = FIRST_GAME_PORT,
                 .supervisorPort = FIRST_SUPERVISED_PLAYER_PORT,
                 .linkToken = FIRST_LINK_TOKEN});

  EXPECT_EQ(
      argumentAfter(command, rtype::network::TRAFFIC_CSV_OPTION),
      (std::filesystem::path{FOLDER_FOR_TESTS} / TRAFFIC_FILE_OF_GAME_THREE)
          .string());
}

/**
 * Given a supervisor launched at the level debug, on the simulated network
 * "bad", with `--traffic-csv traffic.csv`
 * When game 3 reads the command the supervisor builds for it
 * Then the game logs at the level debug, crosses the same network, and
 * writes its traffic to traffic_game_3.csv
 */
TEST(GameCommand,
     TheGameReadsTheLoggingTheNetworkAndTheTrafficOfItsSupervisor) {
  const LaunchOptions supervisor =
      LaunchOptions::read(SUPERVISOR_LEVEL_NETWORK_AND_TRAFFIC);

  const GameLaunchOptions game =
      gameOptionsOf(gameCommand(settingsOf(supervisor), launchOfGameThree()));

  EXPECT_EQ(game.networkConditions(), supervisor.networkConditions());
  EXPECT_TRUE(std::ranges::equal(game.logOptions().forwardedArguments(),
                                 DEBUG_LEVEL_ARGUMENTS));
  EXPECT_EQ(game.trafficCsvFile().value_or(std::filesystem::path{}),
            std::filesystem::path{TRAFFIC_FILE_OF_GAME_THREE});
}

/**
 * Given a supervisor launched with `--no-log-file --no-log-stderr`
 * When game 3 reads the command the supervisor builds for it
 * Then the game writes no journal and nothing to standard error either: the
 * choice of the supervisor wins over the journal of the game
 */
TEST(GameCommand, TheGameWritesNoJournalWhenItsSupervisorWritesNone) {
  const LaunchOptions supervisor =
      LaunchOptions::read(SUPERVISOR_WITHOUT_JOURNAL);

  const GameLaunchOptions game =
      gameOptionsOf(gameCommand(settingsOf(supervisor), launchOfGameThree()));

  EXPECT_TRUE(std::ranges::equal(game.logOptions().forwardedArguments(),
                                 SUPERVISOR_WITHOUT_JOURNAL));
}
