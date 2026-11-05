#include <gtest/gtest.h>
#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "ServersRoutesFixture.hpp"
#include "ServersRoutesTestConstants.hpp"

using rtype::network::heartbeatText;
using rtype::network::isWellFormedServerKey;
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
 * When a host creates a server
 * Then it gets 201, the Location of the server named by its id, and its key
 */
TEST_F(ServersRoutesFixture, ACreationAnswersWhereTheServerLivesAndItsKey) {
  const httplib::Result answer = createParis();

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_CREATED);
  const nlohmann::json created = nlohmann::json::parse(answer->body);
  EXPECT_EQ(answer->get_header_value(std::string{LOCATION_HEADER_NAME}),
            std::string{EXPECTED_SERVER_LOCATION_PREFIX} +
                std::to_string(created.at(ID_MEMBER_NAME).get<std::int64_t>()));
  EXPECT_TRUE(
      isWellFormedServerKey(created.at(KEY_MEMBER_NAME).get<std::string>()));
}

/**
 * Given a master
 * When a host creates a server without a name
 * Then it gets 400, whose detail names the name
 */
TEST_F(ServersRoutesFixture, ACreationWithoutANameNamesTheMissingMember) {
  const httplib::Result answer =
      send(RouteCall{.method = POST_METHOD,
                     .path = SERVERS_URL,
                     .key = {},
                     .body = std::string{EMPTY_OBJECT_BODY}});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
  EXPECT_NE(nlohmann::json::parse(answer->body)
                .at(PROBLEM_DETAIL_MEMBER_NAME)
                .get<std::string>()
                .find(quoted(NAME_POINTER_TEXT)),
            std::string::npos);
}

/**
 * Given the loopback, which created the server of the fixture
 * When it creates four more, then a sixth one
 * Then the four are created and the sixth gets 429
 */
TEST_F(ServersRoutesFixture, ASixthCreationInAnHourIsTooManyRequests) {
  for (std::size_t creation = 1; creation < CREATIONS_PER_HOUR; ++creation) {
    const httplib::Result answer = createParis();
    ASSERT_TRUE(answer);
    ASSERT_EQ(answer->status, EXPECTED_CREATED);
  }

  const httplib::Result answer = createParis();

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_TOO_MANY_REQUESTS);
}

/**
 * Given a created server
 * When it registers with no key
 * Then it gets 401, asked for a Bearer key
 */
TEST_F(ServersRoutesFixture, ARegistrationWithoutAKeyIsAskedForOne) {
  const httplib::Result answer =
      send(RouteCall{.method = PUT_METHOD,
                     .path = OWN_SERVER_URL,
                     .key = {},
                     .body = registrationText(parisRegistration())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_UNAUTHORIZED);
  EXPECT_EQ(answer->get_header_value(std::string{CHALLENGE_HEADER_NAME}),
            EXPECTED_CHALLENGE);
}

/**
 * Given a created server
 * When a server registers with a well-formed key the master never issued
 * Then it gets 401
 */
TEST_F(ServersRoutesFixture, ARegistrationWithAKeyNeverIssuedIsRefused) {
  const httplib::Result answer =
      send(RouteCall{.method = PUT_METHOD,
                     .path = OWN_SERVER_URL,
                     .key = UNKNOWN_SERVER_KEY,
                     .body = registrationText(parisRegistration())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_UNAUTHORIZED);
}

/**
 * Given a registration whose port is written as text, sent with no key
 * When the master reads it
 * Then it gets 401: the key is checked before the body
 */
TEST_F(ServersRoutesFixture, TheKeyIsCheckedBeforeTheBody) {
  const httplib::Result answer = send(
      RouteCall{.method = PUT_METHOD,
                .path = OWN_SERVER_URL,
                .key = {},
                .body = registrationTextWith(UDP_PORT_POINTER, PORT_AS_TEXT)});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_UNAUTHORIZED);
}

/**
 * Given a created server
 * When it registers with its key
 * Then it gets 200 and its representation, under the id of its creation
 */
TEST_F(ServersRoutesFixture, ARegistrationAnswersTheServer) {
  const httplib::Result answer = registerParis();

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(nlohmann::json::parse(answer->body)
                .at(ID_MEMBER_NAME)
                .get<std::int64_t>(),
            createdId());
}

/**
 * Given a registration that names no address
 * When it is sent from the loopback
 * Then the server is registered at the loopback, the address of the peer
 */
TEST_F(ServersRoutesFixture, ARegistrationWithoutAddressTakesThePeer) {
  const httplib::Result answer =
      send(RouteCall{.method = PUT_METHOD,
                     .path = OWN_SERVER_URL,
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
  const httplib::Result answer = send(
      RouteCall{.method = PUT_METHOD,
                .path = OWN_SERVER_URL,
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
 * Given a created server that has not registered
 * When its heartbeat comes
 * Then it gets 409
 */
TEST_F(ServersRoutesFixture, AHeartbeatBeforeTheRegistrationIsAConflict) {
  const httplib::Result answer =
      send(RouteCall{.method = PUT_METHOD,
                     .path = OWN_HEARTBEAT_URL,
                     .key = key(),
                     .body = heartbeatText(parisHeartbeat())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_CONFLICT);
}

/**
 * Given a registered server
 * When its heartbeat comes
 * Then it gets 200 and the orders waiting for it, none yet
 */
TEST_F(ServersRoutesFixture, AHeartbeatGetsTheOrdersWaiting) {
  ASSERT_TRUE(registerParis());

  const httplib::Result answer =
      send(RouteCall{.method = PUT_METHOD,
                     .path = OWN_HEARTBEAT_URL,
                     .key = key(),
                     .body = heartbeatText(parisHeartbeat())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(nlohmann::json::parse(answer->body).at(COMMANDS_MEMBER_NAME),
            nlohmann::json::array());
}
