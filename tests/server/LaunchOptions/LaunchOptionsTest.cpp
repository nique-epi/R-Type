#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "EngineException.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOptions.hpp"
#include "LaunchOptionsTestConstants.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "ServerConstants.hpp"

using rtype::engine::HELP_OPTION;
using rtype::engine::InvalidLaunchOptionValueException;
using rtype::engine::MissingLaunchOptionValueException;
using rtype::engine::PORT_OPTION;
using rtype::engine::UnknownLaunchArgumentException;
using rtype::network::DEFAULT_SERVER_PORT;
using rtype::network::InvalidPortException;
using rtype::server::LaunchOptions;
using rtype::server::SEED_OPTION;

namespace {

using Arguments = std::vector<std::string_view>;

}  // namespace

/**
 * Given no argument
 * When the launch options are read
 * Then the server listens on the default port
 */
TEST(ServerLaunchOptions, NoArgumentGivesTheDefaultPort) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_EQ(options.port(), DEFAULT_SERVER_PORT);
}

/**
 * Given no argument
 * When the launch options are read
 * Then the usage is not asked for
 */
TEST(ServerLaunchOptions, NoArgumentStartsTheServer) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_FALSE(options.helpRequested());
}

/**
 * Given the port option and a port
 * When the launch options are read
 * Then the server listens on that port
 */
TEST(ServerLaunchOptions, PortOptionSetsThePort) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{PORT_OPTION, PORT_ARGUMENT});

  EXPECT_EQ(options.port(), PORT_OF_PORT_ARGUMENT);
}

/**
 * Given the port option twice
 * When the launch options are read
 * Then the last port wins
 */
TEST(ServerLaunchOptions, LastPortOptionWins) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{PORT_OPTION, PORT_ARGUMENT, PORT_OPTION, OTHER_PORT_ARGUMENT});

  EXPECT_EQ(options.port(), PORT_OF_OTHER_PORT_ARGUMENT);
}

/**
 * Given the help option
 * When the launch options are read
 * Then the usage is asked for
 */
TEST(ServerLaunchOptions, HelpOptionAsksForTheUsage) {
  const LaunchOptions options = LaunchOptions::read(Arguments{HELP_OPTION});

  EXPECT_TRUE(options.helpRequested());
}

/**
 * Given the port option as the last argument
 * When the launch options are read
 * Then the missing port is reported
 */
TEST(ServerLaunchOptions, PortOptionWithoutValueIsRejected) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(Arguments{PORT_OPTION})),
               MissingLaunchOptionValueException);
}

/**
 * Given the port option followed by a text that is no port
 * When the launch options are read
 * Then the port is rejected
 */
TEST(ServerLaunchOptions, InvalidPortIsRejected) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(
                   Arguments{PORT_OPTION, NOT_A_PORT_ARGUMENT})),
               InvalidPortException);
}

/**
 * Given an option the server does not define
 * When the launch options are read
 * Then it is rejected
 */
TEST(ServerLaunchOptions, UnknownOptionIsRejected) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{UNKNOWN_OPTION})),
      UnknownLaunchArgumentException);
}

/**
 * Given a port without the option before it
 * When the launch options are read
 * Then it is rejected
 */
TEST(ServerLaunchOptions, ArgumentWithoutOptionIsRejected) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(Arguments{PORT_ARGUMENT})),
               UnknownLaunchArgumentException);
}

/**
 * Given the client's address option, which the server does not take
 * When the launch options are read
 * Then it is rejected instead of being read as a request for help
 */
TEST(ServerLaunchOptions, AddressOptionIsRejected) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{ADDRESS_OPTION})),
      UnknownLaunchArgumentException);
}

/**
 * Given the options of the server and of the logging
 * When the usage is built
 * Then each option starts a line of it
 */
TEST(ServerLaunchOptions, UsageNamesEveryOption) {
  const std::string usage = LaunchOptions::usage();

  for (const std::string_view option :
       {PORT_OPTION, SEED_OPTION, HELP_OPTION, rtype::logging::LEVEL_OPTION,
        rtype::logging::FILE_OPTION, rtype::logging::NO_FILE_OPTION,
        rtype::logging::STDERR_OPTION}) {
    SCOPED_TRACE(option);
    EXPECT_NE(usage.find("\n  " + std::string(option)), std::string::npos);
  }
}

/**
 * Given the default port
 * When the usage is built
 * Then it gives the range of a port and the default one
 */
TEST(ServerLaunchOptions, UsageGivesTheDefaultPort) {
  EXPECT_NE(LaunchOptions::usage().find(DEFAULT_PORT_LINE), std::string::npos);
}

/**
 * Given no argument
 * When the launch options are read
 * Then no seed is given, so the server draws one
 */
TEST(ServerLaunchOptions, NoArgumentGivesNoSeed) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_FALSE(options.seed().has_value());
}

/**
 * Given the seed option and a seed
 * When the launch options are read
 * Then the seed is that number
 */
TEST(ServerLaunchOptions, SeedOptionSetsTheSeed) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{SEED_OPTION, SEED_ARGUMENT});

  EXPECT_EQ(options.seed(), SEED_OF_SEED_ARGUMENT);
}

/**
 * Given the seed option twice
 * When the launch options are read
 * Then the last seed wins
 */
TEST(ServerLaunchOptions, LastSeedOptionWins) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{SEED_OPTION, SEED_ARGUMENT, SEED_OPTION, OTHER_SEED_ARGUMENT});

  EXPECT_EQ(options.seed(), SEED_OF_OTHER_SEED_ARGUMENT);
}

/**
 * Given the seeds 0 and 4294967295, the two ends of the range
 * When the launch options are read
 * Then both are accepted
 */
TEST(ServerLaunchOptions, SeedAcceptsBothEndsOfTheRange) {
  EXPECT_EQ(
      LaunchOptions::read(Arguments{SEED_OPTION, LOWEST_SEED_ARGUMENT}).seed(),
      0U);
  EXPECT_EQ(
      LaunchOptions::read(Arguments{SEED_OPTION, HIGHEST_SEED_ARGUMENT}).seed(),
      HIGHEST_SEED);
}

/**
 * Given a seed one above the range, a negative one, an empty one and a text
 * Then each is rejected
 */
TEST(ServerLaunchOptions, InvalidSeedIsRejected) {
  for (const std::string_view seed :
       {TOO_HIGH_SEED_ARGUMENT, NEGATIVE_SEED_ARGUMENT, EMPTY_SEED_ARGUMENT,
        NOT_A_PORT_ARGUMENT}) {
    EXPECT_THROW(
        static_cast<void>(LaunchOptions::read(Arguments{SEED_OPTION, seed})),
        InvalidLaunchOptionValueException)
        << seed;
  }
}

/**
 * Given the seed option as the last argument
 * When the launch options are read
 * Then the missing seed is reported
 */
TEST(ServerLaunchOptions, SeedOptionWithoutValueIsRejected) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(Arguments{SEED_OPTION})),
               MissingLaunchOptionValueException);
}
