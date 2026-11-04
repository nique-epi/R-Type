#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "ServerKeyTextTestConstants.hpp"
#include "ServersRoutesFixture.hpp"
#include "ServersRoutesTestConstants.hpp"

using rtype::network::heartbeatText;
using rtype::network::registrationText;
using rtype::network::testing::ADDRESS_POINTER;
using rtype::network::testing::parisHeartbeat;
using rtype::network::testing::parisRegistration;
using rtype::network::testing::PORT_AS_TEXT;
using rtype::network::testing::quoted;
using rtype::network::testing::registrationTextWith;
using rtype::network::testing::registrationTextWithout;
using rtype::network::testing::UDP_PORT_POINTER;

/**
 * Given a master
 * When a server registers with no key
 * Then it gets 401, asked for a Bearer key
 */
TEST_F(ServersRoutesFixture, ARegistrationWithoutAKeyIsAskedForOne) {
  const httplib::Result answer =
      put(RouteCall{.path = OWN_SERVER_URL,
                    .key = {},
                    .body = registrationText(parisRegistration())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_UNAUTHORIZED);
  EXPECT_EQ(answer->get_header_value(std::string{CHALLENGE_HEADER_NAME}),
            EXPECTED_CHALLENGE);
}

/**
 * Given a master
 * When a server registers with a well-formed key the master never issued
 * Then it gets 401
 */
TEST_F(ServersRoutesFixture, ARegistrationWithAKeyNeverIssuedIsRefused) {
  const httplib::Result answer =
      put(RouteCall{.path = OWN_SERVER_URL,
                    .key = KEY_NEVER_ISSUED,
                    .body = registrationText(parisRegistration())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_UNAUTHORIZED);
}

/**
 * Given a key that registered no server
 * When a server registers with it
 * Then it gets 201 and the Location of the server, whose id the body carries
 */
TEST_F(ServersRoutesFixture, AFirstRegistrationTellsWhereTheServerLives) {
  const httplib::Result answer = registerParis();

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_CREATED);
  EXPECT_EQ(answer->get_header_value(std::string{LOCATION_HEADER_NAME}),
            std::string{EXPECTED_SERVER_LOCATION_PREFIX} +
                std::to_string(nlohmann::json::parse(answer->body)
                                   .at(ID_MEMBER_NAME)
                                   .get<std::int64_t>()));
}

/**
 * Given a server already registered
 * When it registers again with the same key
 * Then it gets 200 and the same id
 */
TEST_F(ServersRoutesFixture, ARegistrationAgainKeepsTheServer) {
  const httplib::Result first = registerParis();
  ASSERT_TRUE(first);

  const httplib::Result second = registerParis();

  ASSERT_TRUE(second);
  EXPECT_EQ(second->status, EXPECTED_OK);
  EXPECT_EQ(nlohmann::json::parse(second->body).at(ID_MEMBER_NAME),
            nlohmann::json::parse(first->body).at(ID_MEMBER_NAME));
}

/**
 * Given a registration that names no address
 * When it is sent from the loopback
 * Then the server is registered at the loopback, the address of the peer
 */
TEST_F(ServersRoutesFixture, ARegistrationWithoutAddressTakesThePeer) {
  const httplib::Result answer =
      put(RouteCall{.path = OWN_SERVER_URL,
                    .key = key(),
                    .body = registrationTextWithout(ADDRESS_POINTER)});

  ASSERT_TRUE(answer);
  EXPECT_EQ(nlohmann::json::parse(answer->body)
                .at(ADDRESS_MEMBER_NAME)
                .get<std::string>(),
            LOOPBACK_ADDRESS);
}

/**
 * Given a registration whose port is written as text
 * When it is sent with a valid key
 * Then it gets 400, whose detail names the port
 */
TEST_F(ServersRoutesFixture, AnInvalidRegistrationNamesItsFaultyMember) {
  const httplib::Result answer = put(
      RouteCall{.path = OWN_SERVER_URL,
                .key = key(),
                .body = registrationTextWith(UDP_PORT_POINTER, PORT_AS_TEXT)});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
  EXPECT_NE(nlohmann::json::parse(answer->body)
                .at(PROBLEM_DETAIL_MEMBER_NAME)
                .get<std::string>()
                .find(quoted(UDP_PORT_POINTER)),
            std::string::npos);
}

/**
 * Given a key that registered no server
 * When a heartbeat comes with it
 * Then it gets 404
 */
TEST_F(ServersRoutesFixture, AHeartbeatBeforeTheRegistrationIsNotFound) {
  const httplib::Result answer =
      put(RouteCall{.path = OWN_HEARTBEAT_URL,
                    .key = key(),
                    .body = heartbeatText(parisHeartbeat())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_NOT_FOUND);
}

/**
 * Given a registered server
 * When its heartbeat comes
 * Then it gets 200 and the orders waiting for it, none yet
 */
TEST_F(ServersRoutesFixture, AHeartbeatGetsTheOrdersWaiting) {
  ASSERT_TRUE(registerParis());

  const httplib::Result answer =
      put(RouteCall{.path = OWN_HEARTBEAT_URL,
                    .key = key(),
                    .body = heartbeatText(parisHeartbeat())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(nlohmann::json::parse(answer->body).at(COMMANDS_MEMBER_NAME),
            nlohmann::json::array());
}
