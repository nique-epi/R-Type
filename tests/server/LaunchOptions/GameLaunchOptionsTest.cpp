#include <gtest/gtest.h>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "EngineException.hpp"
#include "GameLaunchOptions.hpp"
#include "GameLaunchOptionsTestConstants.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOptionsTestConstants.hpp"
#include "NetworkException.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "SupervisorLink.hpp"

using rtype::engine::DATABASE_OPTION;
using rtype::engine::HELP_OPTION;
using rtype::engine::InvalidLaunchOptionValueException;
using rtype::engine::PORT_OPTION;
using rtype::engine::UnknownLaunchArgumentException;
using rtype::network::InvalidPortException;
using rtype::server::CONFIGURATION_OPTION;
using rtype::server::DIFFICULTY_OPTION;
using rtype::server::FRIENDLY_FIRE_OPTION;
using rtype::server::GameLaunchOptions;
using rtype::server::LINK_TOKEN_OPTION;
using rtype::server::LIVES_OPTION;
using rtype::server::MissingSupervisorLinkException;
using rtype::server::SEED_OPTION;
using rtype::server::SUPERVISOR_OPTION;

using GameArguments = std::vector<std::string_view>;
using Lines = std::vector<std::string>;

/**
 * Given the supervisor option with a port and the link token option with a
 * number
 * When the launch options of a game are read
 * Then the supervisor link holds that port and that token
 */
TEST(GameLaunchOptions, ReadsTheSupervisorLink) {
  const GameLaunchOptions options = GameLaunchOptions::read(
      GameArguments{SUPERVISOR_OPTION, SUPERVISOR_PORT_TEXT, LINK_TOKEN_OPTION,
                    LINK_TOKEN_TEXT});

  EXPECT_EQ(options.supervisorLink().port, SUPERVISOR_PORT_VALUE);
  EXPECT_EQ(options.supervisorLink().token, LINK_TOKEN_VALUE);
}

/**
 * Given the port and database options beside the supervisor link
 * When the launch options of a game are read
 * Then the port and the database settings are forced, in the order given
 */
TEST(GameLaunchOptions, PortAndDatabaseForceTheirSettings) {
  const GameLaunchOptions options = GameLaunchOptions::read(GameArguments{
      PORT_OPTION, PORT_ARGUMENT, SUPERVISOR_OPTION, SUPERVISOR_PORT_TEXT,
      LINK_TOKEN_OPTION, LINK_TOKEN_TEXT, DATABASE_OPTION, DATABASE_ARGUMENT});

  EXPECT_EQ(options.forcedLines(),
            (Lines{std::string(PORT_LINE), std::string(DATABASE_LINE)}));
}

/**
 * Given the three options of the rules of a game beside the supervisor link
 * When the launch options of a game are read
 * Then the difficulty, the lives and the friendly fire are forced, in the
 * order given
 */
TEST(GameLaunchOptions, RuleOptionsForceTheirSettings) {
  const GameLaunchOptions options = GameLaunchOptions::read(
      GameArguments{SUPERVISOR_OPTION, SUPERVISOR_PORT_TEXT, LINK_TOKEN_OPTION,
                    LINK_TOKEN_TEXT, DIFFICULTY_OPTION,
                    HARD_DIFFICULTY_ARGUMENT, LIVES_OPTION, FIVE_LIVES_ARGUMENT,
                    FRIENDLY_FIRE_OPTION, FRIENDLY_FIRE_ON_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), (Lines{std::string(HARD_DIFFICULTY_LINE),
                                          std::string(FIVE_LIVES_LINE),
                                          std::string(FRIENDLY_FIRE_ON_LINE)}));
}

/**
 * Given neither supervisor option
 * When the launch options of a game are read
 * Then the reading is refused, naming the supervisor option
 */
TEST(GameLaunchOptions, RefusesAGameWithoutASupervisorLink) {
  try {
    static_cast<void>(GameLaunchOptions::read(GameArguments{}));
    FAIL() << "the reading did not throw";
  } catch (const MissingSupervisorLinkException& error) {
    EXPECT_NE(std::string_view{error.what()}.find(SUPERVISOR_OPTION),
              std::string_view::npos);
  }
}

/**
 * Given a supervisor port without a link token
 * When the launch options of a game are read
 * Then the reading is refused, naming the link token option
 */
TEST(GameLaunchOptions, RefusesASupervisorWithoutAToken) {
  const GameArguments arguments{SUPERVISOR_OPTION, SUPERVISOR_PORT_TEXT};

  try {
    static_cast<void>(GameLaunchOptions::read(arguments));
    FAIL() << "the reading did not throw";
  } catch (const MissingSupervisorLinkException& error) {
    EXPECT_NE(std::string_view{error.what()}.find(LINK_TOKEN_OPTION),
              std::string_view::npos);
  }
}

/**
 * Given a link token without a supervisor port
 * When the launch options of a game are read
 * Then the reading is refused, naming the supervisor option
 */
TEST(GameLaunchOptions, RefusesATokenWithoutASupervisor) {
  const GameArguments arguments{LINK_TOKEN_OPTION, LINK_TOKEN_TEXT};

  try {
    static_cast<void>(GameLaunchOptions::read(arguments));
    FAIL() << "the reading did not throw";
  } catch (const MissingSupervisorLinkException& error) {
    EXPECT_NE(std::string_view{error.what()}.find(SUPERVISOR_OPTION),
              std::string_view::npos);
  }
}

/**
 * Given the help option alone
 * When the launch options of a game are read
 * Then the usage is asked for, without a supervisor link
 */
TEST(GameLaunchOptions, HelpNeedsNoSupervisorLink) {
  const GameLaunchOptions options =
      GameLaunchOptions::read(GameArguments{HELP_OPTION});

  EXPECT_TRUE(options.helpRequested());
}

/**
 * Given a link token of 0, which is the token of no session
 * When the launch options of a game are read
 * Then the value is refused
 */
TEST(GameLaunchOptions, RefusesALinkTokenOfZero) {
  const GameArguments arguments{SUPERVISOR_OPTION, SUPERVISOR_PORT_TEXT,
                                LINK_TOKEN_OPTION, ZERO_TEXT};

  EXPECT_THROW(static_cast<void>(GameLaunchOptions::read(arguments)),
               InvalidLaunchOptionValueException);
}

/**
 * Given a supervisor port of 0, which no one can reach
 * When the launch options of a game are read
 * Then the port is refused
 */
TEST(GameLaunchOptions, RefusesASupervisorPortOfZero) {
  const GameArguments arguments{SUPERVISOR_OPTION, ZERO_TEXT, LINK_TOKEN_OPTION,
                                LINK_TOKEN_TEXT};

  EXPECT_THROW(static_cast<void>(GameLaunchOptions::read(arguments)),
               InvalidPortException);
}

/**
 * Given the seed option, which the supervisor reads and a game does not
 * When the launch options of a game are read
 * Then it is refused as an unknown argument
 */
TEST(GameLaunchOptions, RefusesTheSeedOption) {
  const GameArguments arguments{SUPERVISOR_OPTION, SUPERVISOR_PORT_TEXT,
                                LINK_TOKEN_OPTION, LINK_TOKEN_TEXT,
                                SEED_OPTION,       SOME_SEED_TEXT};

  EXPECT_THROW(static_cast<void>(GameLaunchOptions::read(arguments)),
               UnknownLaunchArgumentException);
}

/**
 * Given the options of a game
 * When its usage is built
 * Then each of its own options starts a line of it
 */
TEST(GameLaunchOptions, UsageNamesItsOptions) {
  const std::string usage = GameLaunchOptions::usage();

  for (const std::string_view option :
       {PORT_OPTION, SUPERVISOR_OPTION, LINK_TOKEN_OPTION, DATABASE_OPTION,
        DIFFICULTY_OPTION, LIVES_OPTION, FRIENDLY_FIRE_OPTION,
        CONFIGURATION_OPTION, HELP_OPTION}) {
    SCOPED_TRACE(option);
    EXPECT_NE(usage.find("\n  " + std::string(option)), std::string::npos);
  }
}

/**
 * Given the configuration option with a file beside the supervisor link
 * When the launch options of a game are read
 * Then the game reads that file
 */
TEST(GameLaunchOptions, ReadsTheFileOfCommandsItIsGiven) {
  const GameLaunchOptions options = GameLaunchOptions::read(GameArguments{
      SUPERVISOR_OPTION, SUPERVISOR_PORT_TEXT, LINK_TOKEN_OPTION,
      LINK_TOKEN_TEXT, CONFIGURATION_OPTION, GAME_CONFIGURATION_ARGUMENT});

  EXPECT_EQ(options.configurationFile().value_or(std::filesystem::path{}),
            std::filesystem::path{GAME_CONFIGURATION_ARGUMENT});
}

/**
 * Given the supervisor link alone
 * When the launch options of a game are read
 * Then the game reads no file of commands
 */
TEST(GameLaunchOptions, ReadsNoFileOfCommandsWithoutTheOption) {
  const GameLaunchOptions options = GameLaunchOptions::read(
      GameArguments{SUPERVISOR_OPTION, SUPERVISOR_PORT_TEXT, LINK_TOKEN_OPTION,
                    LINK_TOKEN_TEXT});

  EXPECT_EQ(options.configurationFile(), std::nullopt);
}
