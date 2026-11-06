#include <gtest/gtest.h>
#include <string>
#include "InformationConstants.hpp"
#include "ProtocolConstants.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationModuleTestConstants.hpp"
#include "SupervisorModulesFixture.hpp"

using rtype::network::ServerInformation;
using rtype::server::testing::LYON;
using rtype::server::testing::NAME_OF_A_SERVER_LEFT_AS_SHIPPED;
using rtype::server::testing::NAME_THE_SERVER_LYON_LINE;
using rtype::server::testing::NAME_TOO_LONG_LINE;

/**
 * Given a supervisor whose server.cfg leaves server-name as it is
 * When a stranger asks for its information
 * Then it is told the name of server.cfg.example, the game version of the
 * build and the protocol this code speaks
 */
TEST_F(SupervisorModulesFixture, AnswersTheNameTheVersionsAsShipped) {
  const auto application = supervisor({});
  application->start();

  const ServerInformation answer = askInformation(*application);

  EXPECT_EQ(answer.name, NAME_OF_A_SERVER_LEFT_AS_SHIPPED);
  EXPECT_EQ(answer.gameVersion, rtype::server::SERVER_GAME_VERSION);
  EXPECT_EQ(answer.protocolVersion, rtype::network::PROTOCOL_VERSION);
  application->stop();
}

/**
 * Given a server.cfg that names the supervisor Lyon
 * When a stranger asks for its information
 * Then it is told Lyon
 */
TEST_F(SupervisorModulesFixture, AnswersTheNameTheConfigurationGives) {
  writeConfiguration({std::string{NAME_THE_SERVER_LYON_LINE}});
  const auto application = supervisor({});
  application->start();

  EXPECT_EQ(askInformation(*application).name, LYON);
  application->stop();
}

/**
 * Given a server.cfg that gives the supervisor a name of 33 bytes
 * When the supervisor starts
 * Then the line is refused, so the module declared the setting with its rule
 */
TEST_F(SupervisorModulesFixture, RefusesANameTooLong) {
  writeConfiguration({std::string{NAME_TOO_LONG_LINE}});
  const auto application = supervisor({});

  application->start();

  EXPECT_EQ(report().refusals.size(), 1U);
  application->stop();
}
