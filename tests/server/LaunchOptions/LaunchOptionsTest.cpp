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
using rtype::server::DEFAULT_INPUT_DELAY_TICKS;
using rtype::server::DEFAULT_WORLD_STATES_PER_SECOND;
using rtype::server::INPUT_DELAY_OPTION;
using rtype::server::LaunchOptions;
using rtype::server::SEED_OPTION;
using rtype::server::STATE_RATE_OPTION;

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

/**
 * Given no state rate option
 * When the launch options are read
 * Then each player is sent the default number of states per second
 */
TEST(ServerLaunchOptions, NoStateRateOptionGivesTheDefaultRate) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_EQ(options.stateRate(), DEFAULT_WORLD_STATES_PER_SECOND);
}

/**
 * Given the state rate option and a rate
 * When the launch options are read
 * Then each player is sent that many states per second
 */
TEST(ServerLaunchOptions, StateRateOptionSetsTheRate) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{STATE_RATE_OPTION, STATE_RATE_ARGUMENT});

  EXPECT_EQ(options.stateRate(), RATE_OF_STATE_RATE_ARGUMENT);
}

/**
 * Given the state rate option given twice
 * When the launch options are read
 * Then the last rate wins
 */
TEST(ServerLaunchOptions, LastStateRateOptionWins) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{STATE_RATE_OPTION, STATE_RATE_ARGUMENT, STATE_RATE_OPTION,
                OTHER_STATE_RATE_ARGUMENT});

  EXPECT_EQ(options.stateRate(), RATE_OF_OTHER_STATE_RATE_ARGUMENT);
}

/**
 * Given the lowest rate the server accepts
 * When the launch options are read
 * Then it is accepted
 */
TEST(ServerLaunchOptions, StateRateOptionAcceptsTheLowestRate) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{STATE_RATE_OPTION, LOWEST_STATE_RATE_ARGUMENT});

  EXPECT_EQ(options.stateRate(), LOWEST_STATE_RATE);
}

/**
 * Given the highest rate the server accepts, one state per tick
 * When the launch options are read
 * Then it is accepted
 */
TEST(ServerLaunchOptions, StateRateOptionAcceptsTheHighestRate) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{STATE_RATE_OPTION, HIGHEST_STATE_RATE_ARGUMENT});

  EXPECT_EQ(options.stateRate(), HIGHEST_STATE_RATE);
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
 * Given no input delay option
 * When the launch options are read
 * Then each input is held for the default number of ticks
 */
TEST(ServerLaunchOptions, NoInputDelayOptionGivesTheDefaultDelay) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_EQ(options.inputDelay(), DEFAULT_INPUT_DELAY_TICKS);
}

/**
 * Given the input delay option and a delay
 * When the launch options are read
 * Then each input is held for that many ticks
 */
TEST(ServerLaunchOptions, InputDelayOptionSetsTheDelay) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{INPUT_DELAY_OPTION, INPUT_DELAY_ARGUMENT});

  EXPECT_EQ(options.inputDelay(), DELAY_OF_INPUT_DELAY_ARGUMENT);
}

/**
 * Given the input delay option given twice
 * When the launch options are read
 * Then the last delay wins
 */
TEST(ServerLaunchOptions, LastInputDelayOptionWins) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{INPUT_DELAY_OPTION, INPUT_DELAY_ARGUMENT, INPUT_DELAY_OPTION,
                OTHER_INPUT_DELAY_ARGUMENT});

  EXPECT_EQ(options.inputDelay(), DELAY_OF_OTHER_INPUT_DELAY_ARGUMENT);
}

/**
 * Given the lowest delay, none, and the highest delay the server accepts
 * When the launch options are read
 * Then each is accepted
 */
TEST(ServerLaunchOptions, InputDelayOptionAcceptsBothEnds) {
  const LaunchOptions lowest = LaunchOptions::read(
      Arguments{INPUT_DELAY_OPTION, LOWEST_INPUT_DELAY_ARGUMENT});
  const LaunchOptions highest = LaunchOptions::read(
      Arguments{INPUT_DELAY_OPTION, HIGHEST_INPUT_DELAY_ARGUMENT});

  EXPECT_EQ(lowest.inputDelay(), LOWEST_INPUT_DELAY);
  EXPECT_EQ(highest.inputDelay(), HIGHEST_INPUT_DELAY);
}

/**
 * Given a delay over the highest, negative, not a number or empty
 * When the launch options are read
 * Then each is refused as an invalid value of the option
 */
TEST(ServerLaunchOptions, InputDelayOptionRefusesADelayOutOfRange) {
  for (const std::string_view delay :
       {TOO_HIGH_INPUT_DELAY_ARGUMENT, NEGATIVE_INPUT_DELAY_ARGUMENT,
        NOT_A_NUMBER_INPUT_DELAY_ARGUMENT, EMPTY_INPUT_DELAY_ARGUMENT}) {
    SCOPED_TRACE(delay);

    EXPECT_THROW(static_cast<void>(
                     LaunchOptions::read(Arguments{INPUT_DELAY_OPTION, delay})),
                 InvalidLaunchOptionValueException);
  }
}

/**
 * Given the input delay option as the last argument
 * When the launch options are read
 * Then the missing value is reported
 */
TEST(ServerLaunchOptions, InputDelayOptionWithoutValueIsRefused) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{INPUT_DELAY_OPTION})),
      MissingLaunchOptionValueException);
}

/**
 * Given nothing
 * When the usage is asked for
 * Then it names the input delay option
 */
TEST(ServerLaunchOptions, UsageNamesTheInputDelayOption) {
  EXPECT_NE(LaunchOptions::usage().find(std::string(INPUT_DELAY_OPTION)),
            std::string::npos);
}
