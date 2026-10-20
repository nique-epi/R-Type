#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "ClientConstants.hpp"
#include "ClientException.hpp"
#include "Endpoint.hpp"
#include "EngineException.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOptions.hpp"
#include "LaunchOptionsTestConstants.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"

using rtype::client::ADDRESS_OPTION;
using rtype::client::DEFAULT_SERVER_ADDRESS;
using rtype::client::InvalidNicknameOptionException;
using rtype::client::LaunchOptions;
using rtype::client::NICKNAME_OPTION;
using rtype::engine::HELP_OPTION;
using rtype::engine::MissingLaunchOptionValueException;
using rtype::engine::PORT_OPTION;
using rtype::engine::UnknownLaunchArgumentException;
using rtype::network::DEFAULT_SERVER_PORT;
using rtype::network::Endpoint;
using rtype::network::InvalidIpv4AddressException;
using rtype::network::InvalidPortException;

namespace {

using Arguments = std::vector<std::string_view>;

}  // namespace

/**
 * Given no argument
 * When the launch options are read
 * Then the server to join is the default one, on this machine
 */
TEST(ClientLaunchOptions, NoArgumentGivesTheDefaultServer) {
  const LaunchOptions options = LaunchOptions::read(Arguments{});

  EXPECT_EQ(options.serverEndpoint(),
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

  EXPECT_EQ(options.serverEndpoint(),
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

  EXPECT_EQ(options.serverEndpoint(),
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

  EXPECT_EQ(options.serverEndpoint(),
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

  EXPECT_EQ(options.serverEndpoint().address,
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

  EXPECT_EQ(options.serverEndpoint().port, PORT_OF_OTHER_PORT_ARGUMENT);
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
 * Given the options of the client and of the logging
 * When the usage is built
 * Then each option starts a line of it
 */
TEST(ClientLaunchOptions, UsageNamesEveryOption) {
  const std::string usage = LaunchOptions::usage();

  for (const std::string_view option :
       {ADDRESS_OPTION, PORT_OPTION, NICKNAME_OPTION, HELP_OPTION,
        rtype::logging::LEVEL_OPTION, rtype::logging::FILE_OPTION,
        rtype::logging::NO_FILE_OPTION, rtype::logging::STDERR_OPTION,
        rtype::logging::NO_STDERR_OPTION}) {
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

  EXPECT_EQ(options.nickname(), NICKNAME_WHEN_NONE_IS_GIVEN);
}

/**
 * Given the nickname option and a nickname
 * When the launch options are read
 * Then the nickname is that one
 */
TEST(ClientLaunchOptions, NicknameOptionSetsTheNickname) {
  const LaunchOptions options =
      LaunchOptions::read(Arguments{NICKNAME_OPTION, NICKNAME_ARGUMENT});

  EXPECT_EQ(options.nickname(), NICKNAME_ARGUMENT);
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

  EXPECT_EQ(options.nickname(), OTHER_NICKNAME_ARGUMENT);
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
