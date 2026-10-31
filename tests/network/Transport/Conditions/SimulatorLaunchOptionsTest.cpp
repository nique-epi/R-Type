#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "NetworkConditions.hpp"
#include "NetworkException.hpp"
#include "NetworkSimulatorTestConstants.hpp"
#include "SimulatorLaunchOptions.hpp"
#include "SimulatorLaunchOptionsTestConstants.hpp"

using rtype::network::InvalidSimulatorOptionException;
using rtype::network::MissingSimulatorOptionValueException;
using rtype::network::NetworkConditions;
using rtype::network::SimulatorLaunchOptions;

using Arguments = std::vector<std::string_view>;

/**
 * Given no argument
 * When the simulator options are read
 * Then the network is perfect: nothing is simulated
 */
TEST(SimulatorLaunchOptions, NoArgumentSimulatesNothing) {
  EXPECT_EQ(SimulatorLaunchOptions::read(Arguments{}).conditions(),
            NetworkConditions{});
}

/**
 * Given arguments of the program around a simulator option
 * When the simulator options are read
 * Then the program gets every other argument, in the order given
 */
TEST(SimulatorLaunchOptions, KeepsEveryOtherArgumentInItsOrder) {
  const SimulatorLaunchOptions options = SimulatorLaunchOptions::read(
      Arguments{PORT_OPTION_TEXT, PORT_ARGUMENT, LAG_OPTION_TEXT, LAG_ARGUMENT,
                NO_SOUND_OPTION_TEXT});

  EXPECT_EQ(Arguments(options.otherArguments().begin(),
                      options.otherArguments().end()),
            (Arguments{PORT_OPTION_TEXT, PORT_ARGUMENT, NO_SOUND_OPTION_TEXT}));
}

/**
 * Given the lag option and a number of milliseconds
 * When the simulator options are read
 * Then only the lag is set
 */
TEST(SimulatorLaunchOptions, LagOptionSetsTheLag) {
  EXPECT_EQ(
      SimulatorLaunchOptions::read(Arguments{LAG_OPTION_TEXT, LAG_ARGUMENT})
          .conditions(),
      LAG_OF_LAG_ARGUMENT);
}

/**
 * Given the jitter option and a number of milliseconds
 * When the simulator options are read
 * Then only the jitter is set
 */
TEST(SimulatorLaunchOptions, JitterOptionSetsTheJitter) {
  EXPECT_EQ(SimulatorLaunchOptions::read(
                Arguments{JITTER_OPTION_TEXT, JITTER_ARGUMENT})
                .conditions(),
            JITTER_OF_JITTER_ARGUMENT);
}

/**
 * Given the loss option and a percent
 * When the simulator options are read
 * Then only the loss is set
 */
TEST(SimulatorLaunchOptions, LossOptionSetsTheLoss) {
  EXPECT_EQ(
      SimulatorLaunchOptions::read(Arguments{LOSS_OPTION_TEXT, LOSS_ARGUMENT})
          .conditions(),
      LOSS_OF_LOSS_ARGUMENT);
}

/**
 * Given the duplicates option and a percent
 * When the simulator options are read
 * Then only the duplicates are set
 */
TEST(SimulatorLaunchOptions, DuplicatesOptionSetsTheDuplicates) {
  EXPECT_EQ(SimulatorLaunchOptions::read(
                Arguments{DUPLICATES_OPTION_TEXT, DUPLICATES_ARGUMENT})
                .conditions(),
            DUPLICATES_OF_DUPLICATES_ARGUMENT);
}

/**
 * Given the reorder option and a percent
 * When the simulator options are read
 * Then only the reordering is set
 */
TEST(SimulatorLaunchOptions, ReorderOptionSetsTheReordering) {
  EXPECT_EQ(SimulatorLaunchOptions::read(
                Arguments{REORDER_OPTION_TEXT, REORDER_ARGUMENT})
                .conditions(),
            REORDER_OF_REORDER_ARGUMENT);
}

/**
 * Given the network option and the name 4g
 * When the simulator options are read
 * Then the five values are those of 4g in the table
 */
TEST(SimulatorLaunchOptions, NetworkOptionSetsTheValuesOf4g) {
  EXPECT_EQ(SimulatorLaunchOptions::read(
                Arguments{NETWORK_OPTION_TEXT, MOBILE_4G_NAME_TEXT})
                .conditions(),
            MOBILE_4G_OF_THE_TABLE);
}

/**
 * Given the network option and the name wifi-busy
 * When the simulator options are read
 * Then the five values are those of wifi-busy in the table
 */
TEST(SimulatorLaunchOptions, NetworkOptionSetsTheValuesOfABusyWifi) {
  EXPECT_EQ(SimulatorLaunchOptions::read(
                Arguments{NETWORK_OPTION_TEXT, BUSY_WIFI_NAME_TEXT})
                .conditions(),
            BUSY_WIFI_OF_THE_TABLE);
}

/**
 * Given the network option and the name bad
 * When the simulator options are read
 * Then the five values are those of bad in the table
 */
TEST(SimulatorLaunchOptions, NetworkOptionSetsTheValuesOfABadNetwork) {
  EXPECT_EQ(SimulatorLaunchOptions::read(
                Arguments{NETWORK_OPTION_TEXT, BAD_NAME_TEXT})
                .conditions(),
            BAD_OF_THE_TABLE);
}

/**
 * Given the network option, then the loss option
 * When the simulator options are read
 * Then the values are those of the scenario, with the loss given after it
 */
TEST(SimulatorLaunchOptions, AnOptionAfterTheNetworkChangesOneOfItsValues) {
  EXPECT_EQ(
      SimulatorLaunchOptions::read(Arguments{NETWORK_OPTION_TEXT, BAD_NAME_TEXT,
                                             LOSS_OPTION_TEXT, LOSS_ARGUMENT})
          .conditions(),
      BAD_WITH_LOSS_OF_LOSS_ARGUMENT);
}

/**
 * Given the loss option, then the network option
 * When the simulator options are read
 * Then the values are those of the scenario: it was given last
 */
TEST(SimulatorLaunchOptions, TheNetworkAfterAnOptionReplacesItsValue) {
  EXPECT_EQ(SimulatorLaunchOptions::read(
                Arguments{LOSS_OPTION_TEXT, LOSS_ARGUMENT, NETWORK_OPTION_TEXT,
                          BAD_NAME_TEXT})
                .conditions(),
            BAD_OF_THE_TABLE);
}

/**
 * Given the network option and a name no scenario has
 * When the simulator options are read
 * Then they are refused
 */
TEST(SimulatorLaunchOptions, RefusesAnUnknownNetwork) {
  EXPECT_THROW(static_cast<void>(SimulatorLaunchOptions::read(
                   Arguments{NETWORK_OPTION_TEXT, UNKNOWN_SCENARIO_ARGUMENT})),
               InvalidSimulatorOptionException);
}

/**
 * Given the lag option and 10 000 milliseconds, the longest delay
 * When the simulator options are read
 * Then the lag is 10 000 milliseconds
 */
TEST(SimulatorLaunchOptions, AcceptsTheLongestDelay) {
  EXPECT_EQ(SimulatorLaunchOptions::read(
                Arguments{LAG_OPTION_TEXT, LONGEST_DELAY_ARGUMENT})
                .conditions(),
            LONGEST_LAG);
}

/**
 * Given the jitter option and 10 001 milliseconds
 * When the simulator options are read
 * Then they are refused
 */
TEST(SimulatorLaunchOptions, RefusesADelayLongerThanTheLongest) {
  EXPECT_THROW(static_cast<void>(SimulatorLaunchOptions::read(
                   Arguments{JITTER_OPTION_TEXT, TOO_LONG_DELAY_ARGUMENT})),
               InvalidSimulatorOptionException);
}

/**
 * Given the loss option and 100 percent
 * When the simulator options are read
 * Then every datagram is lost
 */
TEST(SimulatorLaunchOptions, AcceptsAFullShare) {
  EXPECT_EQ(SimulatorLaunchOptions::read(
                Arguments{LOSS_OPTION_TEXT, FULL_SHARE_ARGUMENT})
                .conditions(),
            FULL_LOSS_OF_THE_OPTIONS);
}

/**
 * Given the duplicates option and 101 percent
 * When the simulator options are read
 * Then they are refused
 */
TEST(SimulatorLaunchOptions, RefusesAShareAboveAHundred) {
  EXPECT_THROW(static_cast<void>(SimulatorLaunchOptions::read(Arguments{
                   DUPLICATES_OPTION_TEXT, TOO_LARGE_SHARE_ARGUMENT})),
               InvalidSimulatorOptionException);
}

/**
 * Given the reorder option and 0 percent
 * When the simulator options are read
 * Then the network is perfect
 */
TEST(SimulatorLaunchOptions, AcceptsZero) {
  EXPECT_EQ(SimulatorLaunchOptions::read(
                Arguments{REORDER_OPTION_TEXT, ZERO_ARGUMENT})
                .conditions(),
            NetworkConditions{});
}

/**
 * Given the lag option and a negative number
 * When the simulator options are read
 * Then they are refused
 */
TEST(SimulatorLaunchOptions, RefusesANegativeValue) {
  EXPECT_THROW(static_cast<void>(SimulatorLaunchOptions::read(
                   Arguments{LAG_OPTION_TEXT, NEGATIVE_ARGUMENT})),
               InvalidSimulatorOptionException);
}

/**
 * Given the loss option and a word
 * When the simulator options are read
 * Then they are refused
 */
TEST(SimulatorLaunchOptions, RefusesAValueThatIsNoNumber) {
  EXPECT_THROW(static_cast<void>(SimulatorLaunchOptions::read(
                   Arguments{LOSS_OPTION_TEXT, NOT_A_NUMBER_ARGUMENT})),
               InvalidSimulatorOptionException);
}

/**
 * Given the lag option and an empty value
 * When the simulator options are read
 * Then they are refused
 */
TEST(SimulatorLaunchOptions, RefusesAnEmptyValue) {
  EXPECT_THROW(static_cast<void>(SimulatorLaunchOptions::read(
                   Arguments{LAG_OPTION_TEXT, EMPTY_ARGUMENT})),
               InvalidSimulatorOptionException);
}

/**
 * Given the lag option as the last argument
 * When the simulator options are read
 * Then they are refused: the value is missing
 */
TEST(SimulatorLaunchOptions, RefusesAnOptionWithoutItsValue) {
  EXPECT_THROW(static_cast<void>(
                   SimulatorLaunchOptions::read(Arguments{LAG_OPTION_TEXT})),
               MissingSimulatorOptionValueException);
}

/**
 * Given nothing
 * When the usage of the simulator options is built
 * Then it starts a line with each option and names each scenario
 */
TEST(SimulatorLaunchOptions, UsageNamesEveryOptionAndScenario) {
  const std::string usage = SimulatorLaunchOptions::usage();

  for (const std::string_view option : SIMULATOR_OPTION_TEXTS) {
    SCOPED_TRACE(option);
    EXPECT_NE(usage.find("\n  " + std::string(option)), std::string::npos);
  }
  for (const std::string_view scenario :
       {MOBILE_4G_NAME_TEXT, BUSY_WIFI_NAME_TEXT, BAD_NAME_TEXT}) {
    SCOPED_TRACE(scenario);
    EXPECT_NE(usage.find(std::string(scenario) + " ("), std::string::npos);
  }
}
