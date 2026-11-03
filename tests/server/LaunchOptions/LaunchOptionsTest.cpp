#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include "EngineException.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOptions.hpp"
#include "LaunchOptionsTestConstants.hpp"
#include "LogLaunchOptionsFixture.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "LoggingException.hpp"
#include "LoggingOptionTestConstants.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "NetworkSimulatorTestConstants.hpp"
#include "ServerConstants.hpp"
#include "SimulatorLaunchOptions.hpp"
#include "TrafficFileTestConstants.hpp"

using rtype::engine::HELP_OPTION;
using rtype::engine::InvalidLaunchOptionValueException;
using rtype::engine::MissingLaunchOptionValueException;
using rtype::engine::PORT_OPTION;
using rtype::engine::SET_OPTION;
using rtype::engine::UnknownLaunchArgumentException;
using rtype::logging::InvalidLogOptionException;
using rtype::logging::LEVEL_OPTION;
using rtype::logging::Logger;
using rtype::logging::LogLevel;
using rtype::logging::MissingLogOptionValueException;
using rtype::logging::NO_FILE_OPTION;
using rtype::logging::NO_STDERR_OPTION;
using rtype::network::InvalidPortException;
using rtype::network::TRAFFIC_CSV_OPTION;
using rtype::server::DATABASE_OPTION;
using rtype::server::INPUT_DELAY_OPTION;
using rtype::server::LaunchOptions;
using rtype::server::SEED_OPTION;
using rtype::server::STATE_RATE_OPTION;

using Arguments = std::vector<std::string_view>;
using Lines = std::vector<std::string>;

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
       {PORT_OPTION, SEED_OPTION, DATABASE_OPTION, HELP_OPTION,
        rtype::logging::LEVEL_OPTION, rtype::logging::FILE_OPTION,
        rtype::logging::NO_FILE_OPTION, rtype::logging::STDERR_OPTION,
        rtype::logging::NO_STDERR_OPTION}) {
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
 * Given a set option whose value is not `name=value`
 * When the launch options are read
 * Then each is refused as an invalid value of the option
 */
TEST(ServerLaunchOptions, SetOptionRefusesAnInvalidAssignment) {
  for (const std::string_view assignment :
       {ASSIGNMENT_WITHOUT_MARK, ASSIGNMENT_WITHOUT_NAME}) {
    EXPECT_THROW(static_cast<void>(
                     LaunchOptions::read(Arguments{SET_OPTION, assignment})),
                 InvalidLaunchOptionValueException)
        << assignment;
  }
}

/**
 * Given no argument
 * When the launch options are read
 * Then the network of the server is perfect: nothing is simulated
 */
TEST(ServerLaunchOptions, NoArgumentSimulatesNoNetwork) {
  EXPECT_EQ(LaunchOptions::read(Arguments{}).networkConditions(),
            rtype::network::NetworkConditions{});
}

/**
 * Given the port option and a port, then the network option and a scenario
 * When the launch options are read
 * Then the port is forced as a setting, behind the network of that scenario
 */
TEST(ServerLaunchOptions, ReadsTheNetworkSimulatorOptionsBesideItsOwn) {
  const LaunchOptions options = LaunchOptions::read(Arguments{
      PORT_OPTION, PORT_ARGUMENT, NETWORK_OPTION_TEXT, BAD_NAME_TEXT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string(PORT_LINE)});
  EXPECT_EQ(options.networkConditions(), BAD_OF_THE_TABLE);
}

/**
 * Given the seed option, then a logging option and its value, then a seed
 * When the launch options are read
 * Then the logging option is the value of the seed option, and is refused as
 *      a seed
 */
TEST(ServerLaunchOptions, SeedOptionTakesALoggingOptionAsItsValue) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{
          SEED_OPTION, LEVEL_OPTION, DEBUG_LEVEL_ARGUMENT, SEED_ARGUMENT})),
      InvalidLaunchOptionValueException);
}

/**
 * Given the seed option, then a simulator option and its value, then a seed
 * When the launch options are read
 * Then the simulator option is the value of the seed option, and is refused
 *      as a seed
 */
TEST(ServerLaunchOptions, SeedOptionTakesASimulatorOptionAsItsValue) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(Arguments{
                   SEED_OPTION, LAG_OPTION_TEXT, LAG_ARGUMENT, SEED_ARGUMENT})),
               InvalidLaunchOptionValueException);
}

/**
 * Given the port option and a port, then the logging level option and a
 * level, with no journal and nothing on standard error
 * When the launch options are read and their logging options applied
 * Then the port is forced as a setting and the logger is at that level
 */
TEST_F(LogLaunchOptionsFixture, ServerReadsTheLoggingOptionsBesideItsOwn) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{PORT_OPTION, PORT_ARGUMENT, LEVEL_OPTION, DEBUG_LEVEL_ARGUMENT,
                NO_FILE_OPTION, NO_STDERR_OPTION});

  options.logOptions().apply();

  EXPECT_EQ(options.forcedLines(), Lines{std::string(PORT_LINE)});
  EXPECT_EQ(Logger::level(), LogLevel::Debug);
}

/**
 * Given the logging level option, misspelled
 * When the launch options are read
 * Then it is refused as a logging option that does not exist
 */
TEST(ServerLaunchOptions, RefusesAMisspelledLoggingOption) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(
                   Arguments{MISSPELLED_LEVEL_OPTION, DEBUG_LEVEL_ARGUMENT})),
               InvalidLogOptionException);
}

/**
 * Given the logging level option as the last argument
 * When the launch options are read
 * Then the missing value is reported as the logging options report it
 */
TEST(ServerLaunchOptions, LoggingOptionWithoutValueIsRefused) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(Arguments{LEVEL_OPTION})),
               MissingLogOptionValueException);
}

/**
 * Given no input delay option
 * When the launch options are read
 * Then no line is forced, so the setting keeps its default
 */
TEST(ServerLaunchOptions, NoInputDelayOptionForcesNoLine) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_TRUE(options.forcedLines().empty());
}

/**
 * Given the input delay option and a delay
 * When the launch options are read
 * Then the line that gives the input delay setting that delay is forced
 */
TEST(ServerLaunchOptions, InputDelayOptionForcesTheSetting) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{INPUT_DELAY_OPTION, INPUT_DELAY_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string(INPUT_DELAY_LINE)});
}

/**
 * Given the input delay option given twice
 * When the launch options are read
 * Then both lines are forced in order, so the last delay wins
 */
TEST(ServerLaunchOptions, LastInputDelayOptionWins) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{INPUT_DELAY_OPTION, INPUT_DELAY_ARGUMENT, INPUT_DELAY_OPTION,
                OTHER_INPUT_DELAY_ARGUMENT});

  EXPECT_EQ(options.forcedLines(),
            (Lines{std::string(INPUT_DELAY_LINE),
                   std::string(OTHER_INPUT_DELAY_LINE)}));
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

  EXPECT_EQ(lowest.forcedLines(), Lines{std::string(LOWEST_INPUT_DELAY_LINE)});
  EXPECT_EQ(highest.forcedLines(),
            Lines{std::string(HIGHEST_INPUT_DELAY_LINE)});
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
 * When the usage is built
 * Then it holds the whole part that describes the network simulator
 */
TEST(ServerLaunchOptions, UsageHoldsTheNetworkSimulatorOptions) {
  EXPECT_NE(LaunchOptions::usage().find(
                rtype::network::SimulatorLaunchOptions::usage()),
            std::string::npos);
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

/**
 * Given no argument
 * When the launch options are read
 * Then no traffic file is asked for
 */
TEST(ServerLaunchOptions, NoArgumentAsksForNoTrafficFile) {
  EXPECT_FALSE(LaunchOptions::read(Arguments{}).trafficCsvFile().has_value());
}

/**
 * Given the traffic option and a file
 * When the launch options are read
 * Then the file is the one given
 */
TEST(ServerLaunchOptions, TrafficOptionGivesTheFile) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{TRAFFIC_CSV_OPTION, TRAFFIC_FILE_NAME});

  EXPECT_EQ(options.trafficCsvFile().value_or(std::filesystem::path{}),
            std::filesystem::path{TRAFFIC_FILE_NAME});
}

/**
 * Given the usage of the server
 * When it is read
 * Then it lists the traffic option
 */
TEST(ServerLaunchOptions, UsageListsTheTrafficOption) {
  EXPECT_NE(LaunchOptions::usage().find(TRAFFIC_CSV_OPTION), std::string::npos);
}

/**
 * Given the database option and a file
 * When the launch options are read
 * Then the database setting is forced to that file
 */
TEST(ServerLaunchOptions, DatabaseOptionForcesTheDatabase) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{DATABASE_OPTION, DATABASE_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string(DATABASE_LINE)});
}

/**
 * Given the database option and a file whose name holds a space
 * When the launch options are read
 * Then the file reaches the setting as one quoted word
 */
TEST(ServerLaunchOptions, DatabaseOptionKeepsAFileWithASpaceWhole) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{DATABASE_OPTION, DATABASE_ARGUMENT_WITH_A_SPACE});

  EXPECT_EQ(options.forcedLines(),
            Lines{std::string(DATABASE_LINE_WITH_A_SPACE)});
}

/**
 * Given the database option as the last argument
 * When the launch options are read
 * Then the missing file is reported
 */
TEST(ServerLaunchOptions, DatabaseOptionWithoutValueIsRejected) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{DATABASE_OPTION})),
      MissingLaunchOptionValueException);
}
