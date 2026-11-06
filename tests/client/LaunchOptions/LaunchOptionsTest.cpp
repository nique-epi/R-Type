#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include "AudioMode.hpp"
#include "ClientConstants.hpp"
#include "ClientException.hpp"
#include "ConnectionRequest.hpp"
#include "Endpoint.hpp"
#include "EngineException.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOptions.hpp"
#include "LaunchOptionsTestConstants.hpp"
#include "LogLaunchOptionsFixture.hpp"
#include "LogLevel.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "LoggingOptionTestConstants.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "NetworkSimulatorTestConstants.hpp"
#include "SimulatorLaunchOptions.hpp"
#include "TrafficFileTestConstants.hpp"

using rtype::client::ADDRESS_OPTION;
using rtype::client::AudioMode;
using rtype::client::DEFAULT_SERVER_ADDRESS;
using rtype::client::GAME_OPTION;
using rtype::client::InvalidGameOptionException;
using rtype::client::InvalidNicknameOptionException;
using rtype::client::LaunchOptions;
using rtype::client::NICKNAME_OPTION;
using rtype::client::NO_SOUND_OPTION;
using rtype::engine::HELP_OPTION;
using rtype::engine::InvalidLaunchOptionValueException;
using rtype::engine::MissingLaunchOptionValueException;
using rtype::engine::PORT_OPTION;
using rtype::engine::SET_OPTION;
using rtype::engine::UnknownLaunchArgumentException;
using rtype::logging::LEVEL_OPTION;
using rtype::logging::Logger;
using rtype::logging::LogLevel;
using rtype::logging::NO_FILE_OPTION;
using rtype::logging::NO_STDERR_OPTION;
using rtype::network::DEFAULT_SERVER_PORT;
using rtype::network::Endpoint;
using rtype::network::InvalidIpv4AddressException;
using rtype::network::InvalidPortException;
using rtype::network::TRAFFIC_CSV_OPTION;

using Arguments = std::vector<std::string_view>;
using Lines = std::vector<std::string>;

/**
 * Given no argument
 * When the launch options are read
 * Then the server to join is the default one, on this machine
 */
TEST(ClientLaunchOptions, NoArgumentGivesTheDefaultServer) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_EQ(options.serverChoice().server,
            (Endpoint{.address = std::string(DEFAULT_SERVER_ADDRESS),
                      .port = DEFAULT_SERVER_PORT}));
}

/**
 * Given no argument
 * When the launch options are read
 * Then the usage is not asked for
 */
TEST(ClientLaunchOptions, NoArgumentStartsTheClient) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_FALSE(options.helpRequested());
}

/**
 * Given the address option and an IPv4 address
 * When the launch options are read
 * Then the server to join has that address and the default port
 */
TEST(ClientLaunchOptions, AddressOptionSetsTheAddress) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{ADDRESS_OPTION, ADDRESS_ARGUMENT});

  EXPECT_EQ(options.serverChoice().server,
            (Endpoint{.address = std::string(ADDRESS_ARGUMENT),
                      .port = DEFAULT_SERVER_PORT}));
}

/**
 * Given the port option and a port
 * When the launch options are read
 * Then the server to join has that port and the default address
 */
TEST(ClientLaunchOptions, PortOptionSetsThePort) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{PORT_OPTION, PORT_ARGUMENT});

  EXPECT_EQ(options.serverChoice().server,
            (Endpoint{.address = std::string(DEFAULT_SERVER_ADDRESS),
                      .port = PORT_OF_PORT_ARGUMENT}));
}

/**
 * Given the port option before the address option
 * When the launch options are read
 * Then the server to join has both
 */
TEST(ClientLaunchOptions, AddressAndPortCanComeInAnyOrder) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{PORT_OPTION, PORT_ARGUMENT, ADDRESS_OPTION, ADDRESS_ARGUMENT});

  EXPECT_EQ(options.serverChoice().server,
            (Endpoint{.address = std::string(ADDRESS_ARGUMENT),
                      .port = PORT_OF_PORT_ARGUMENT}));
}

/**
 * Given the address option twice
 * When the launch options are read
 * Then the last address wins
 */
TEST(ClientLaunchOptions, LastAddressOptionWins) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{ADDRESS_OPTION, ADDRESS_ARGUMENT,
                                    ADDRESS_OPTION, OTHER_ADDRESS_ARGUMENT});

  EXPECT_EQ(options.serverChoice().server.address,
            std::string(OTHER_ADDRESS_ARGUMENT));
}

/**
 * Given the port option twice
 * When the launch options are read
 * Then the last port wins
 */
TEST(ClientLaunchOptions, LastPortOptionWins) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{PORT_OPTION, PORT_ARGUMENT, PORT_OPTION, OTHER_PORT_ARGUMENT});

  EXPECT_EQ(options.serverChoice().server.port, PORT_OF_OTHER_PORT_ARGUMENT);
}

/**
 * Given the help option
 * When the launch options are read
 * Then the usage is asked for
 */
TEST(ClientLaunchOptions, HelpOptionAsksForTheUsage) {
  const LaunchOptions options = LaunchOptions::read(Arguments{HELP_OPTION});

  EXPECT_TRUE(options.helpRequested());
}

/**
 * Given the address option as the last argument
 * When the launch options are read
 * Then the missing address is reported
 */
TEST(ClientLaunchOptions, AddressOptionWithoutValueIsRejected) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{ADDRESS_OPTION})),
      MissingLaunchOptionValueException);
}

/**
 * Given the port option as the last argument
 * When the launch options are read
 * Then the missing port is reported
 */
TEST(ClientLaunchOptions, PortOptionWithoutValueIsRejected) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(Arguments{PORT_OPTION})),
               MissingLaunchOptionValueException);
}

/**
 * Given the address option followed by a host name
 * When the launch options are read
 * Then the address is rejected instead of being resolved
 */
TEST(ClientLaunchOptions, HostNameIsRejected) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(
                   Arguments{ADDRESS_OPTION, HOST_NAME_ARGUMENT})),
               InvalidIpv4AddressException);
}

/**
 * Given the port option followed by a text that is no port
 * When the launch options are read
 * Then the port is rejected
 */
TEST(ClientLaunchOptions, InvalidPortIsRejected) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(
                   Arguments{PORT_OPTION, NOT_A_PORT_ARGUMENT})),
               InvalidPortException);
}

/**
 * Given an option the client does not define
 * When the launch options are read
 * Then it is rejected
 */
TEST(ClientLaunchOptions, UnknownOptionIsRejected) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{UNKNOWN_OPTION})),
      UnknownLaunchArgumentException);
}

/**
 * Given an address without the option before it
 * When the launch options are read
 * Then it is rejected
 */
TEST(ClientLaunchOptions, ArgumentWithoutOptionIsRejected) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{ADDRESS_ARGUMENT})),
      UnknownLaunchArgumentException);
}

/**
 * Given no argument
 * When the launch options are read
 * Then the client makes sound
 */
TEST(ClientLaunchOptions, NoArgumentKeepsTheSoundOn) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_EQ(options.audioMode(), AudioMode::Enabled);
}

/**
 * Given the no-sound option
 * When the launch options are read
 * Then the client makes no sound
 */
TEST(ClientLaunchOptions, NoSoundOptionTurnsTheSoundOff) {
  const LaunchOptions options = LaunchOptions::read(Arguments{NO_SOUND_OPTION});

  EXPECT_EQ(options.audioMode(), AudioMode::Disabled);
}

/**
 * Given the no-sound option and the address option
 * When the launch options are read
 * Then the sound is off and the address is still read
 */
TEST(ClientLaunchOptions, NoSoundOptionLeavesTheOtherOptionsAlone) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{NO_SOUND_OPTION, ADDRESS_OPTION, ADDRESS_ARGUMENT});

  EXPECT_EQ(options.serverChoice().server.address,
            std::string(ADDRESS_ARGUMENT));
}

/**
 * Given the options of the client and of the logging
 * When the usage is built
 * Then each option starts a line of it
 */
TEST(ClientLaunchOptions, UsageNamesEveryOption) {
  const std::string usage = LaunchOptions::usage();

  for (const std::string_view option :
       {ADDRESS_OPTION, PORT_OPTION, NICKNAME_OPTION, GAME_OPTION,
        NO_SOUND_OPTION, HELP_OPTION, rtype::logging::LEVEL_OPTION,
        rtype::logging::FILE_OPTION, rtype::logging::NO_FILE_OPTION,
        rtype::logging::STDERR_OPTION, rtype::logging::NO_STDERR_OPTION}) {
    SCOPED_TRACE(option);
    EXPECT_NE(usage.find("\n  " + std::string(option)), std::string::npos);
  }
}

/**
 * Given the default address
 * When the usage is built
 * Then it tells what an address is and gives the default one
 */
TEST(ClientLaunchOptions, UsageGivesTheDefaultAddress) {
  EXPECT_NE(LaunchOptions::usage().find(DEFAULT_ADDRESS_LINE),
            std::string::npos);
}

/**
 * Given the default port
 * When the usage is built
 * Then it gives the range of a port and the default one
 */
TEST(ClientLaunchOptions, UsageGivesTheDefaultPort) {
  EXPECT_NE(LaunchOptions::usage().find(DEFAULT_PORT_LINE), std::string::npos);
}

/**
 * Given no argument
 * When the launch options are read
 * Then the nickname is the one of a player who gives none
 */
TEST(ClientLaunchOptions, NoArgumentGivesTheDefaultNickname) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_EQ(options.serverChoice().nickname, NICKNAME_WHEN_NONE_IS_GIVEN);
}

/**
 * Given the nickname option and a nickname
 * When the launch options are read
 * Then the nickname is that one
 */
TEST(ClientLaunchOptions, NicknameOptionSetsTheNickname) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{NICKNAME_OPTION, NICKNAME_ARGUMENT});

  EXPECT_EQ(options.serverChoice().nickname, NICKNAME_ARGUMENT);
}

/**
 * Given the nickname option twice
 * When the launch options are read
 * Then the last nickname wins
 */
TEST(ClientLaunchOptions, LastNicknameOptionWins) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{NICKNAME_OPTION, NICKNAME_ARGUMENT,
                                    NICKNAME_OPTION, OTHER_NICKNAME_ARGUMENT});

  EXPECT_EQ(options.serverChoice().nickname, OTHER_NICKNAME_ARGUMENT);
}

/**
 * Given the nickname option and a nickname of 17 bytes
 * When the launch options are read
 * Then the nickname is rejected
 */
TEST(ClientLaunchOptions, SeventeenByteNicknameIsRejected) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(
                   Arguments{NICKNAME_OPTION, SEVENTEEN_BYTE_NICKNAME})),
               InvalidNicknameOptionException);
}

/**
 * Given the nickname option as the last argument
 * When the launch options are read
 * Then the missing nickname is reported
 */
TEST(ClientLaunchOptions, NicknameOptionWithoutValueIsRejected) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{NICKNAME_OPTION})),
      MissingLaunchOptionValueException);
}

/**
 * Given the default nickname
 * When the usage is built
 * Then it gives the size of a nickname and the default one
 */
TEST(ClientLaunchOptions, UsageGivesTheDefaultNickname) {
  EXPECT_NE(LaunchOptions::usage().find(DEFAULT_NICKNAME_LINE),
            std::string::npos);
}

/**
 * Given no argument
 * When the launch options are read
 * Then no setting is forced, so the configuration file and the defaults decide
 */
TEST(ClientLaunchOptions, NoArgumentForcesNoSetting) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_TRUE(options.forcedLines().empty());
}

/**
 * Given the address, the port and the nickname options
 * When the launch options are read
 * Then each setting is forced, in the order the options were given
 */
TEST(ClientLaunchOptions, EachOptionForcesTheSettingItGives) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{ADDRESS_OPTION, ADDRESS_ARGUMENT, PORT_OPTION, PORT_ARGUMENT,
                NICKNAME_OPTION, NICKNAME_ARGUMENT});

  EXPECT_EQ(options.forcedLines(),
            (Lines{std::string(ADDRESS_LINE), std::string(PORT_LINE),
                   std::string(NICKNAME_LINE)}));
}

/**
 * Given a nickname holding a space and quotes
 * When the launch options are read
 * Then the forced line quotes and escapes it
 */
TEST(ClientLaunchOptions, NicknameWithSpacesAndQuotesIsForcedWhole) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{NICKNAME_OPTION, AWKWARD_NICKNAME_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string(AWKWARD_NICKNAME_LINE)});
}

/**
 * Given the set option and `name=value`
 * When the launch options are read
 * Then the line that gives the setting that value is forced
 */
TEST(ClientLaunchOptions, SetOptionForcesTheSettingItNames) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{SET_OPTION, ASSIGNMENT_ARGUMENT});

  EXPECT_EQ(options.forcedLines(), Lines{std::string(NICKNAME_LINE)});
}

/**
 * Given a set option whose value is not `name=value`
 * When the launch options are read
 * Then each is refused as an invalid value of the option
 */
TEST(ClientLaunchOptions, SetOptionRefusesAnInvalidAssignment) {
  for (const std::string_view assignment :
       {ASSIGNMENT_WITHOUT_MARK, ASSIGNMENT_WITHOUT_NAME}) {
    EXPECT_THROW(static_cast<void>(
                     LaunchOptions::read(Arguments{SET_OPTION, assignment})),
                 InvalidLaunchOptionValueException)
        << assignment;
  }
}

/**
 * Given the usage
 * When it is read
 * Then it names the set option
 */
TEST(ClientLaunchOptions, UsageNamesTheSetOption) {
  EXPECT_NE(LaunchOptions::usage().find(std::string(SET_OPTION)),
            std::string::npos);
}

/**
 * Given no argument
 * When the launch options are read
 * Then the network of the client is perfect: nothing is simulated
 */
TEST(ClientLaunchOptions, NoArgumentSimulatesNoNetwork) {
  EXPECT_EQ(LaunchOptions::read(Arguments{}).networkConditions(),
            rtype::network::NetworkConditions{});
}

/**
 * Given the address option and an address, then the lag option and a lag
 * When the launch options are read
 * Then the client joins that address, behind a network with that lag
 */
TEST(ClientLaunchOptions, ReadsTheNetworkSimulatorOptionsBesideItsOwn) {
  const LaunchOptions options = LaunchOptions::read(Arguments{
      ADDRESS_OPTION, ADDRESS_ARGUMENT, LAG_OPTION_TEXT, LAG_ARGUMENT});

  EXPECT_EQ(options.serverChoice().server.address, ADDRESS_ARGUMENT);
  EXPECT_EQ(options.networkConditions(), LAG_OF_LAG_ARGUMENT);
}

/**
 * Given the address option, then a logging option and its value, then an
 * address
 * When the launch options are read
 * Then the logging option is the value of the address option, and is refused
 *      as an address
 */
TEST(ClientLaunchOptions, AddressOptionTakesALoggingOptionAsItsValue) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(
                   Arguments{ADDRESS_OPTION, LEVEL_OPTION, DEBUG_LEVEL_ARGUMENT,
                             ADDRESS_ARGUMENT})),
               InvalidIpv4AddressException);
}

/**
 * Given the address option, then a simulator option and its value, then an
 * address
 * When the launch options are read
 * Then the simulator option is the value of the address option, and is
 *      refused as an address
 */
TEST(ClientLaunchOptions, AddressOptionTakesASimulatorOptionAsItsValue) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{
          ADDRESS_OPTION, LAG_OPTION_TEXT, LAG_ARGUMENT, ADDRESS_ARGUMENT})),
      InvalidIpv4AddressException);
}

/**
 * Given the port option, then a logging option and its value, then a port
 * When the launch options are read
 * Then the logging option is the value of the port option, and is refused as
 *      a port
 */
TEST(ClientLaunchOptions, PortOptionTakesALoggingOptionAsItsValue) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{
          PORT_OPTION, LEVEL_OPTION, DEBUG_LEVEL_ARGUMENT, PORT_ARGUMENT})),
      InvalidPortException);
}

/**
 * Given the port option, then a simulator option and its value, then a port
 * When the launch options are read
 * Then the simulator option is the value of the port option, and is refused
 *      as a port
 */
TEST(ClientLaunchOptions, PortOptionTakesASimulatorOptionAsItsValue) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(Arguments{
                   PORT_OPTION, LAG_OPTION_TEXT, LAG_ARGUMENT, PORT_ARGUMENT})),
               InvalidPortException);
}

/**
 * Given the nickname option, then a logging option and its value, then a
 * nickname
 * When the launch options are read
 * Then the logging option is the nickname, so the level after it is an
 *      argument no option names, and is refused
 */
TEST(ClientLaunchOptions, NicknameOptionTakesALoggingOptionAsItsValue) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(
                   Arguments{NICKNAME_OPTION, LEVEL_OPTION,
                             DEBUG_LEVEL_ARGUMENT, NICKNAME_ARGUMENT})),
               UnknownLaunchArgumentException);
}

/**
 * Given the nickname option, then a simulator option and its value, then a
 * nickname
 * When the launch options are read
 * Then the simulator option is the nickname, so the lag after it is an
 *      argument no option names, and is refused
 */
TEST(ClientLaunchOptions, NicknameOptionTakesASimulatorOptionAsItsValue) {
  EXPECT_THROW(
      static_cast<void>(LaunchOptions::read(Arguments{
          NICKNAME_OPTION, LAG_OPTION_TEXT, LAG_ARGUMENT, NICKNAME_ARGUMENT})),
      UnknownLaunchArgumentException);
}

/**
 * Given the nickname option, then a logging option that takes no value
 * When the launch options are read
 * Then the logging option is the nickname of the player
 */
TEST(ClientLaunchOptions, NicknameOptionTakesALoggingFlagAsTheNickname) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{NICKNAME_OPTION, NO_FILE_OPTION});

  EXPECT_EQ(options.serverChoice().nickname, NO_FILE_OPTION);
}

/**
 * Given the port option and a port, then the logging level option and a
 * level, with no journal and nothing on standard error
 * When the launch options are read and their logging options applied
 * Then the client joins that port and the logger is at that level
 */
TEST_F(LogLaunchOptionsFixture, ClientReadsTheLoggingOptionsBesideItsOwn) {
  const LaunchOptions options = LaunchOptions::read(
      Arguments{PORT_OPTION, PORT_ARGUMENT, LEVEL_OPTION, DEBUG_LEVEL_ARGUMENT,
                NO_FILE_OPTION, NO_STDERR_OPTION});

  options.logOptions().apply();

  EXPECT_EQ(options.serverChoice().server.port, PORT_OF_PORT_ARGUMENT);
  EXPECT_EQ(Logger::level(), LogLevel::Debug);
}

/**
 * Given nothing
 * When the usage is built
 * Then it holds the whole part that describes the network simulator
 */
TEST(ClientLaunchOptions, UsageHoldsTheNetworkSimulatorOptions) {
  EXPECT_NE(LaunchOptions::usage().find(
                rtype::network::SimulatorLaunchOptions::usage()),
            std::string::npos);
}

/**
 * Given no argument
 * When the launch options are read
 * Then no traffic file is asked for
 */
TEST(ClientLaunchOptions, NoArgumentAsksForNoTrafficFile) {
  EXPECT_FALSE(LaunchOptions::read(Arguments{}).trafficCsvFile().has_value());
}

/**
 * Given the traffic option and a file
 * When the launch options are read
 * Then the file is the one given
 */
TEST(ClientLaunchOptions, TrafficOptionGivesTheFile) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{TRAFFIC_CSV_OPTION, TRAFFIC_FILE_NAME});

  EXPECT_EQ(options.trafficCsvFile().value_or(std::filesystem::path{}),
            std::filesystem::path{TRAFFIC_FILE_NAME});
}

/**
 * Given the usage of the client
 * When it is read
 * Then it lists the traffic option
 */
TEST(ClientLaunchOptions, UsageListsTheTrafficOption) {
  EXPECT_NE(LaunchOptions::usage().find(TRAFFIC_CSV_OPTION), std::string::npos);
}

/**
 * Given no argument
 * When the launch options are read
 * Then the client asks for any game
 */
TEST(ClientLaunchOptions, NoArgumentAsksForAnyGame) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_EQ(options.serverChoice().game, rtype::network::ANY_GAME_CHOICE);
}

/**
 * Given the game option and the value new
 * When the launch options are read
 * Then the client asks for a new game
 */
TEST(ClientLaunchOptions, GameOptionNewAsksForANewGame) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{GAME_OPTION, NEW_GAME_ARGUMENT});

  EXPECT_EQ(options.serverChoice().game, rtype::network::NEW_GAME_CHOICE);
}

/**
 * Given the game option and the number 3
 * When the launch options are read
 * Then the client asks for game 3
 */
TEST(ClientLaunchOptions, GameOptionNumberAsksForThatGame) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{GAME_OPTION, GAME_THREE_ARGUMENT});

  EXPECT_EQ(options.serverChoice().game,
            (rtype::network::GameChoice{
                .kind = rtype::network::GameChoiceKind::ExistingGame,
                .number = GAME_THREE_NUMBER}));
}

/**
 * Given the game option and the number 0, below the first game
 * When the launch options are read
 * Then the game is rejected
 */
TEST(ClientLaunchOptions, GameZeroIsRejected) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(
                   Arguments{GAME_OPTION, GAME_ZERO_ARGUMENT})),
               InvalidGameOptionException);
}

/**
 * Given the game option and the number 65536, past the last number
 * When the launch options are read
 * Then the game is rejected
 */
TEST(ClientLaunchOptions, GamePastTheLastNumberIsRejected) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(
                   Arguments{GAME_OPTION, GAME_PAST_THE_LAST_ARGUMENT})),
               InvalidGameOptionException);
}

/**
 * Given the game option and a word other than new
 * When the launch options are read
 * Then the game is rejected
 */
TEST(ClientLaunchOptions, GameWordIsRejected) {
  EXPECT_THROW(static_cast<void>(LaunchOptions::read(
                   Arguments{GAME_OPTION, WORD_GAME_ARGUMENT})),
               InvalidGameOptionException);
}
