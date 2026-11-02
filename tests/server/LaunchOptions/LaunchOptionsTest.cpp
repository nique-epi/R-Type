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
using rtype::engine::SET_OPTION;
using rtype::engine::UnknownLaunchArgumentException;
using rtype::network::InvalidPortException;
using rtype::server::LaunchOptions;
using rtype::server::SEED_OPTION;
using rtype::server::STATE_RATE_OPTION;

namespace {

using Arguments = std::vector<std::string_view>;
using Lines = std::vector<std::string>;

}  // namespace

/**
 * Given no argument
 * When the launch options are read
 * Then no setting is forced, so the configuration file and the defaults decide
 */
TEST(ServerLaunchOptions, NoArgumentForcesNoSetting) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_TRUE(options.forcedLines().empty());
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
 * Then the port setting is forced to that port
 */
TEST(ServerLaunchOptions, PortOptionForcesThePort) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{PORT_OPTION, PORT_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string(PORT_LINE)});
}

/**
 * Given the port option twice
 * When the launch options are read
 * Then both lines are forced in the order given, so the last port wins when
 * they run
 */
TEST(ServerLaunchOptions, PortOptionTwiceForcesBothInOrder) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{PORT_OPTION, PORT_ARGUMENT, PORT_OPTION, OTHER_PORT_ARGUMENT});

  EXPECT_EQ(options.forcedLines(),
            (Lines{std::string(PORT_LINE), std::string(OTHER_PORT_LINE)}));
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
        rtype::logging::STDERR_OPTION, rtype::logging::NO_STDERR_OPTION}) {
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
 * Given the seed option and a seed
 * When the launch options are read
 * Then the seed setting is forced to that number
 */
TEST(ServerLaunchOptions, SeedOptionForcesTheSeed) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{SEED_OPTION, SEED_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string(SEED_LINE)});
}

/**
 * Given the seed option twice
 * When the launch options are read
 * Then both lines are forced in the order given
 */
TEST(ServerLaunchOptions, SeedOptionTwiceForcesBothInOrder) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{SEED_OPTION, SEED_ARGUMENT, SEED_OPTION, OTHER_SEED_ARGUMENT});

  EXPECT_EQ(options.forcedLines(),
            (Lines{std::string(SEED_LINE), std::string(OTHER_SEED_LINE)}));
}

/**
 * Given the seeds 0 and 4294967295, the two ends of the range
 * When the launch options are read
 * Then both are accepted and forced
 */
TEST(ServerLaunchOptions, SeedAcceptsBothEndsOfTheRange) {
  EXPECT_EQ(LaunchOptions::read(Arguments{SEED_OPTION, LOWEST_SEED_ARGUMENT})
                .forcedLines(),
            Lines{std::string(LOWEST_SEED_LINE)});
  EXPECT_EQ(LaunchOptions::read(Arguments{SEED_OPTION, HIGHEST_SEED_ARGUMENT})
                .forcedLines(),
            Lines{std::string(HIGHEST_SEED_LINE)});
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

/**
 * Given the state rate option and a rate
 * When the launch options are read
 * Then the state rate setting is forced to that rate
 */
TEST(ServerLaunchOptions, StateRateOptionForcesTheRate) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{STATE_RATE_OPTION, STATE_RATE_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string(STATE_RATE_LINE)});
}

/**
 * Given the state rate option given twice
 * When the launch options are read
 * Then both lines are forced in the order given
 */
TEST(ServerLaunchOptions, StateRateOptionTwiceForcesBothInOrder) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{STATE_RATE_OPTION, STATE_RATE_ARGUMENT, STATE_RATE_OPTION,
                OTHER_STATE_RATE_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), (Lines{std::string(STATE_RATE_LINE),
                                          std::string(OTHER_STATE_RATE_LINE)}));
}

/**
 * Given the lowest rate the server accepts
 * When the launch options are read
 * Then it is accepted and forced
 */
TEST(ServerLaunchOptions, StateRateOptionAcceptsTheLowestRate) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{STATE_RATE_OPTION, LOWEST_STATE_RATE_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string(LOWEST_STATE_RATE_LINE)});
}

/**
 * Given the highest rate the server accepts, one state per tick
 * When the launch options are read
 * Then it is accepted and forced
 */
TEST(ServerLaunchOptions, StateRateOptionAcceptsTheHighestRate) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{STATE_RATE_OPTION, HIGHEST_STATE_RATE_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string(HIGHEST_STATE_RATE_LINE)});
}

/**
 * Given a rate under the lowest, over the highest, negative, not a number or
 * empty
 * When the launch options are read
 * Then each is refused as an invalid value of the option
 */
TEST(ServerLaunchOptions, StateRateOptionRefusesARateOutOfRange) {
  for (const std::string_view rate :
       {TOO_LOW_STATE_RATE_ARGUMENT, TOO_HIGH_STATE_RATE_ARGUMENT,
        NEGATIVE_STATE_RATE_ARGUMENT, NOT_A_NUMBER_STATE_RATE_ARGUMENT,
        EMPTY_STATE_RATE_ARGUMENT}) {
    SCOPED_TRACE(rate);

    EXPECT_THROW(static_cast<void>(
                     LaunchOptions::read(Arguments{STATE_RATE_OPTION, rate})),
                 InvalidLaunchOptionValueException);
  }
}

/**
 * Given the state rate option as the last argument
 * When the launch options are read
 * Then the missing value is reported
 */
TEST(ServerLaunchOptions, StateRateOptionWithoutValueIsRefused) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{STATE_RATE_OPTION})),
      MissingLaunchOptionValueException);
}

/**
 * Given nothing
 * When the usage is asked for
 * Then it names the state rate option
 */
TEST(ServerLaunchOptions, UsageNamesTheStateRateOption) {
  EXPECT_NE(LaunchOptions::usage().find(std::string(STATE_RATE_OPTION)),
            std::string::npos);
}

/**
 * Given the set option and `name=value`
 * When the launch options are read
 * Then the line that gives the setting that value is forced
 */
TEST(ServerLaunchOptions, SetOptionForcesTheSettingItNames) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{SET_OPTION, ASSIGNMENT_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string(ASSIGNMENT_LINE)});
}

/**
 * Given the port option, the set option and the seed option
 * When the launch options are read
 * Then the lines are forced in the order the options were given
 */
TEST(ServerLaunchOptions, ForcedLinesKeepTheOrderOfTheOptions) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{PORT_OPTION, PORT_ARGUMENT, SET_OPTION, ASSIGNMENT_ARGUMENT,
                SEED_OPTION, SEED_ARGUMENT});

  EXPECT_EQ(options.forcedLines(),
            (Lines{std::string(PORT_LINE), std::string(ASSIGNMENT_LINE),
                   std::string(SEED_LINE)}));
}

/**
 * Given a set option whose value is not `name=value`, or holds a quote
 * When the launch options are read
 * Then each is refused as an invalid value of the option
 */
TEST(ServerLaunchOptions, SetOptionRefusesAnInvalidAssignment) {
  for (const std::string_view assignment :
       {ASSIGNMENT_WITHOUT_MARK, ASSIGNMENT_WITHOUT_NAME,
        ASSIGNMENT_WITH_QUOTE}) {
    EXPECT_THROW(static_cast<void>(
                     LaunchOptions::read(Arguments{SET_OPTION, assignment})),
                 InvalidLaunchOptionValueException)
        << assignment;
  }
}

/**
 * Given the set option as the last argument
 * When the launch options are read
 * Then the missing value is reported
 */
TEST(ServerLaunchOptions, SetOptionWithoutValueIsRefused) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(Arguments{SET_OPTION})),
               MissingLaunchOptionValueException);
}

/**
 * Given the usage
 * When it is read
 * Then it names the set option
 */
TEST(ServerLaunchOptions, UsageNamesTheSetOption) {
  EXPECT_NE(LaunchOptions::usage().find(std::string(SET_OPTION)),
            std::string::npos);
}
