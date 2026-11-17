#include <gtest/gtest.h>
#include <httplib.h>
#include <string>
#include "AccountsRoutesFixtures.hpp"
#include "GameTicketRoutesTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "WalletRoutesFixture.hpp"
#include "WalletRoutesFixtures.hpp"

/**
 * Given ACE, signed in
 * When it asks a game ticket over HTTPS, then reads where the answer says the
 * ticket lives
 * Then the master answers 201 with a well-formed ticket, its expiry and its
 * place, which names it without holding it, and the ticket reads back unused
 */
TEST_F(WalletRoutesFixture, ATicketIsCreatedAndReadBack) {
  const httplib::Result created =
      send(ownCall(POST_METHOD, OWN_TICKETS_URL, token()));
  ASSERT_TRUE(created);
  ASSERT_EQ(created->status, EXPECTED_CREATED);
  const std::string ticket = memberIn(created, TICKET_MEMBER_NAME);
  EXPECT_TRUE(rtype::network::isWellFormedGameTicket(ticket));
  EXPECT_TRUE(numberIn(created, EXPIRES_AT_MEMBER_NAME).has_value());
  const std::string location =
      created->get_header_value(std::string{LOCATION_HEADER_NAME});
  EXPECT_TRUE(location.starts_with(TICKET_LOCATION_START));
  EXPECT_EQ(location.find(ticket.substr(TICKET_PREFIX_SIZE)),
            std::string::npos);

  const httplib::Result read =
      send(ownCall(GET_METHOD_TEXT, location, token()));

  ASSERT_TRUE(read);
  EXPECT_EQ(read->status, EXPECTED_OK);
  EXPECT_NE(read->body.find(UNUSED_MEMBER_TEXT), std::string::npos);
}

/**
 * Given a master with wallets
 * When a client asks a game ticket without a token
 * Then the master answers 401
 */
TEST_F(WalletRoutesFixture, RefusesATicketWithoutASession) {
  const httplib::Result refused =
      send(ownCall(POST_METHOD, OWN_TICKETS_URL, {}));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_UNAUTHORIZED);
}

/**
 * Given a master with wallets
 * When ACE asks a game ticket in plain HTTP
 * Then the master answers 403, so neither the token nor the ticket crosses in
 * clear
 */
TEST_F(WalletRoutesFixture, RefusesATicketInPlainHttp) {
  RouteCall call = ownCall(POST_METHOD, OWN_TICKETS_URL, token());
  call.overHttps = false;

  const httplib::Result refused = send(call);

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_FORBIDDEN);
}

/**
 * Given ACE, who was given no ticket
 * When it reads a ticket by a fingerprint
 * Then the master answers 404
 */
TEST_F(WalletRoutesFixture, RefusesATicketTheAccountWasNeverGiven) {
  const httplib::Result refused =
      send(ownCall(GET_METHOD_TEXT, UNKNOWN_TICKET_URL, token()));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_NOT_FOUND);
}
