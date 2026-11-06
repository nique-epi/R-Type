#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <vector>
#include "GameCommand.hpp"
#include "GameLaunch.hpp"
#include "GameLaunchSettings.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::server::gameCommand;
using rtype::server::GameLaunch;
using rtype::server::GameLaunchSettings;

/**
 * Given the settings of a supervisor on port 4242 and the launch of game 3 on
 * port 50123 under the token 99
 * When the command of the game is built
 * Then it starts the program as a game process, with its port, its supervisor,
 * its token, the leaderboard of the supervisor's port and its own journal file
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
                (folder / "r-type_game_3.log").string()}));
}
