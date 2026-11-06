#include <gtest/gtest.h>
#include <span>
#include <string_view>
#include <vector>
#include "EngineException.hpp"
#include "ServerMode.hpp"
#include "ServerModeChoice.hpp"
#include "ServerModeOption.hpp"
#include "ServerModeOptionTestConstants.hpp"

using rtype::engine::InvalidLaunchOptionValueException;
using rtype::engine::MissingLaunchOptionValueException;
using rtype::server::chooseServerMode;
using rtype::server::ServerMode;
using rtype::server::ServerModeChoice;

using Arguments = std::vector<std::string_view>;

/**
 * Given arguments that do not start with `--mode`
 * When the mode is chosen
 * Then the binary runs the supervisor with every argument
 */
TEST(ServerModeOption, WithoutTheModeTheSupervisorRunsWithEveryArgument) {
  const Arguments arguments{PORT, SOME_PORT};

  const ServerModeChoice choice = chooseServerMode(arguments);

  EXPECT_EQ(choice.mode, ServerMode::Supervisor);
  EXPECT_EQ(Arguments(choice.options.begin(), choice.options.end()), arguments);
}

/**
 * Given `--mode master` followed by an option
 * When the mode is chosen
 * Then the binary runs the master with the arguments after the mode
 */
TEST(ServerModeOption, ModeMasterRunsTheMasterWithTheArgumentsAfterIt) {
  const Arguments arguments{MODE, MASTER_MODE, PORT, SOME_PORT};

  const ServerModeChoice choice = chooseServerMode(arguments);

  EXPECT_EQ(choice.mode, ServerMode::Master);
  EXPECT_EQ(Arguments(choice.options.begin(), choice.options.end()),
            (Arguments{PORT, SOME_PORT}));
}

/**
 * Given `--mode supervisor` followed by an option
 * When the mode is chosen
 * Then the binary runs the supervisor with the arguments after the mode
 */
TEST(ServerModeOption, ModeSupervisorRunsTheSupervisorWithTheArgumentsAfterIt) {
  const Arguments arguments{MODE, SUPERVISOR_MODE, PORT, SOME_PORT};

  const ServerModeChoice choice = chooseServerMode(arguments);

  EXPECT_EQ(choice.mode, ServerMode::Supervisor);
  EXPECT_EQ(Arguments(choice.options.begin(), choice.options.end()),
            (Arguments{PORT, SOME_PORT}));
}

/**
 * Given `--mode game` alone
 * When the mode is chosen
 * Then the binary runs a game process with no option
 */
TEST(ServerModeOption, ModeGameRunsAGameProcess) {
  const Arguments arguments{MODE, GAME_MODE};

  const ServerModeChoice choice = chooseServerMode(arguments);

  EXPECT_EQ(choice.mode, ServerMode::Game);
  EXPECT_TRUE(choice.options.empty());
}

/**
 * Given `--mode server`
 * When the mode is chosen
 * Then it is refused: a server that hosts several games in one process is no
 * program of the binary
 */
TEST(ServerModeOption, ServerIsNotAMode) {
  const Arguments arguments{MODE, SERVER_MODE};

  EXPECT_THROW(static_cast<void>(chooseServerMode(arguments)),
               InvalidLaunchOptionValueException);
}

/**
 * Given `--mode` followed by a name that is no mode
 * When the mode is chosen
 * Then it is refused
 */
TEST(ServerModeOption, AnUnknownModeIsRefused) {
  const Arguments arguments{MODE, UNKNOWN_MODE};

  EXPECT_THROW(static_cast<void>(chooseServerMode(arguments)),
               InvalidLaunchOptionValueException);
}

/**
 * Given `--mode` as the last argument
 * When the mode is chosen
 * Then it is refused for its missing value
 */
TEST(ServerModeOption, AModeWithoutItsValueIsRefused) {
  const Arguments arguments{MODE};

  EXPECT_THROW(static_cast<void>(chooseServerMode(arguments)),
               MissingLaunchOptionValueException);
}

/**
 * Given `--mode master` after another option
 * When the mode is chosen
 * Then the binary runs the supervisor with every argument, whose options
 * refuse the mode: it must come first
 */
TEST(ServerModeOption, AModeAfterAnotherOptionIsLeftToTheSupervisor) {
  const Arguments arguments{PORT, SOME_PORT, MODE, MASTER_MODE};

  const ServerModeChoice choice = chooseServerMode(arguments);

  EXPECT_EQ(choice.mode, ServerMode::Supervisor);
  EXPECT_EQ(choice.options.size(), arguments.size());
}
