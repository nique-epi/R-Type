#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "EngineException.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "MasterConstants.hpp"
#include "MasterLaunchOptions.hpp"
#include "MasterLaunchOptionsTestConstants.hpp"
#include "NetworkException.hpp"

using rtype::engine::HELP_OPTION;
using rtype::engine::PORT_OPTION;
using rtype::engine::UnknownLaunchArgumentException;
using rtype::master::DATABASE_OPTION;
using rtype::master::MasterLaunchOptions;
using rtype::network::InvalidPortException;

using Arguments = std::vector<std::string_view>;
using Lines = std::vector<std::string>;

/**
 * Given no argument
 * When the launch options of the master are read
 * Then no setting is forced and no usage is asked for
 */
TEST(MasterLaunchOptions, NoArgumentForcesNothing) {
  const MasterLaunchOptions options = MasterLaunchOptions::read(Arguments{});

  EXPECT_TRUE(options.forcedLines().empty());
  EXPECT_FALSE(options.helpRequested());
}

/**
 * Given `-p 8081`
 * When the launch options of the master are read
 * Then they force the port setting to 8081
 */
TEST(MasterLaunchOptions, APortForcesThePortSetting) {
  const MasterLaunchOptions options =
      MasterLaunchOptions::read(Arguments{PORT_OPTION, GIVEN_PORT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string{FORCED_PORT_LINE}});
}

/**
 * Given `-p` with the lowest, then the highest port
 * When the launch options of the master are read
 * Then both are accepted
 */
TEST(MasterLaunchOptions, TheBoundsOfThePortsAreAccepted) {
  EXPECT_NO_THROW(static_cast<void>(
      MasterLaunchOptions::read(Arguments{PORT_OPTION, LOWEST_ACCEPTED_PORT})));
  EXPECT_NO_THROW(static_cast<void>(MasterLaunchOptions::read(
      Arguments{PORT_OPTION, HIGHEST_ACCEPTED_PORT})));
}

/**
 * Given `-p` with a port just below, then just above the ports of TCP
 * When the launch options of the master are read
 * Then both are refused, before anything starts
 */
TEST(MasterLaunchOptions, APortOutsideTheRangeIsRefused) {
  EXPECT_THROW(static_cast<void>(MasterLaunchOptions::read(
                   Arguments{PORT_OPTION, PORT_BELOW_THE_RANGE})),
               InvalidPortException);
  EXPECT_THROW(static_cast<void>(MasterLaunchOptions::read(
                   Arguments{PORT_OPTION, PORT_ABOVE_THE_RANGE})),
               InvalidPortException);
}

/**
 * Given `--database` and a file
 * When the launch options of the master are read
 * Then they force the database setting to that file
 */
TEST(MasterLaunchOptions, ADatabaseForcesTheDatabaseSetting) {
  const MasterLaunchOptions options =
      MasterLaunchOptions::read(Arguments{DATABASE_OPTION, GIVEN_DATABASE});

  EXPECT_EQ(options.forcedLines(), Lines{std::string{FORCED_DATABASE_LINE}});
}

/**
 * Given `--help`
 * When the launch options of the master are read
 * Then the usage is asked for
 */
TEST(MasterLaunchOptions, HelpAsksForTheUsage) {
  const MasterLaunchOptions options =
      MasterLaunchOptions::read(Arguments{HELP_OPTION});

  EXPECT_TRUE(options.helpRequested());
}

/**
 * Given an option of the client, which the master does not take
 * When the launch options of the master are read
 * Then it is refused
 */
TEST(MasterLaunchOptions, AnOptionOfAnotherProgramIsRefused) {
  EXPECT_THROW(static_cast<void>(MasterLaunchOptions::read(
                   Arguments{CLIENT_ADDRESS_OPTION, CLIENT_ADDRESS})),
               UnknownLaunchArgumentException);
}
