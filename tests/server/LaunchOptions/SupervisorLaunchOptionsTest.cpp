#include <gtest/gtest.h>
#include <string_view>
#include <vector>
#include "EngineException.hpp"
#include "LaunchOptions.hpp"
#include "NetworkException.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "SupervisorLaunchOptionsTestConstants.hpp"
#include "SupervisorLink.hpp"

using rtype::engine::InvalidLaunchOptionValueException;
using rtype::network::InvalidPortException;
using rtype::server::IncompleteSupervisorLinkException;
using rtype::server::LaunchOptions;
using rtype::server::LINK_TOKEN_OPTION;
using rtype::server::SUPERVISOR_OPTION;
using rtype::server::SupervisorLink;

using LinkArguments = std::vector<std::string_view>;

/**
 * Given the supervisor option with a port and the link token option with a
 * number
 * When the launch options are read
 * Then the supervisor link holds that port and that token
 */
TEST(SupervisorLaunchOptions, ReadsTheSupervisorLink) {
  const LaunchOptions options =
      LaunchOptions::read(LinkArguments{SUPERVISOR_OPTION, SUPERVISOR_PORT_TEXT,
                                        LINK_TOKEN_OPTION, LINK_TOKEN_TEXT});

  const SupervisorLink link =
      options.supervisorLink().value_or(SupervisorLink{});
  ASSERT_TRUE(options.supervisorLink().has_value());
  EXPECT_EQ(link.port, SUPERVISOR_PORT_VALUE);
  EXPECT_EQ(link.token, LINK_TOKEN_VALUE);
}

/**
 * Given neither supervisor option
 * When the launch options are read
 * Then the server has no supervisor
 */
TEST(SupervisorLaunchOptions, HasNoLinkWithoutTheOptions) {
  const LaunchOptions options = LaunchOptions::read(LinkArguments{});

  EXPECT_FALSE(options.supervisorLink().has_value());
}

/**
 * Given a supervisor port without a link token
 * When the launch options are read
 * Then the reading is refused, naming the link token option
 */
TEST(SupervisorLaunchOptions, RefusesASupervisorWithoutAToken) {
  const LinkArguments arguments{SUPERVISOR_OPTION, SUPERVISOR_PORT_TEXT};

  try {
    static_cast<void>(LaunchOptions::read(arguments));
    FAIL() << "the reading did not throw";
  } catch (const IncompleteSupervisorLinkException& error) {
    EXPECT_NE(std::string_view{error.what()}.find(LINK_TOKEN_OPTION),
              std::string_view::npos);
  }
}

/**
 * Given a link token without a supervisor port
 * When the launch options are read
 * Then the reading is refused, naming the supervisor option
 */
TEST(SupervisorLaunchOptions, RefusesATokenWithoutASupervisor) {
  const LinkArguments arguments{LINK_TOKEN_OPTION, LINK_TOKEN_TEXT};

  try {
    static_cast<void>(LaunchOptions::read(arguments));
    FAIL() << "the reading did not throw";
  } catch (const IncompleteSupervisorLinkException& error) {
    EXPECT_NE(std::string_view{error.what()}.find(SUPERVISOR_OPTION),
              std::string_view::npos);
  }
}

/**
 * Given a link token of 0, which is the token of no session
 * When the launch options are read
 * Then the value is refused
 */
TEST(SupervisorLaunchOptions, RefusesALinkTokenOfZero) {
  const LinkArguments arguments{SUPERVISOR_OPTION, SUPERVISOR_PORT_TEXT,
                                LINK_TOKEN_OPTION, ZERO_TEXT};

  EXPECT_THROW(static_cast<void>(LaunchOptions::read(arguments)),
               InvalidLaunchOptionValueException);
}

/**
 * Given a supervisor port of 0, which no one can reach
 * When the launch options are read
 * Then the port is refused
 */
TEST(SupervisorLaunchOptions, RefusesASupervisorPortOfZero) {
  const LinkArguments arguments{SUPERVISOR_OPTION, ZERO_TEXT, LINK_TOKEN_OPTION,
                                LINK_TOKEN_TEXT};

  EXPECT_THROW(static_cast<void>(LaunchOptions::read(arguments)),
               InvalidPortException);
}
