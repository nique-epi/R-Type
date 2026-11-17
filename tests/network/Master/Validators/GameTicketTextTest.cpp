#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include "GameTicketGrant.hpp"
#include "GameTicketTextTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "NetworkException.hpp"

using rtype::network::GameTicketGrant;
using rtype::network::gameTicketGrantFromText;
using rtype::network::gameTicketGrantText;
using rtype::network::InvalidMasterMessageException;
using rtype::network::isWellFormedGameTicket;

/**
 * Given a ticket the master gives, and its expiry
 * When it is written as the master answers it, then read back
 * Then it reads as the same ticket and expiry
 */
TEST(GameTicketText, ReadsBackAWrittenTicket) {
  const GameTicketGrant grant{.ticket = std::string{WELL_FORMED_TICKET},
                              .expiresAt = TICKET_EXPIRY};

  EXPECT_EQ(gameTicketGrantFromText(gameTicketGrantText(grant)), grant);
}

/**
 * Given a ticket of the shape the master issues
 * When it is checked
 * Then it is well formed
 */
TEST(GameTicketText, AcceptsATicketOfTheShapeTheMasterIssues) {
  EXPECT_TRUE(isWellFormedGameTicket(WELL_FORMED_TICKET));
}

/**
 * Given tickets with the prefix of a session token, in uppercase, one digit
 * short, or one digit too many
 * When each is checked
 * Then none is well formed
 */
TEST(GameTicketText, RefusesATicketShapedAlmostRight) {
  for (const std::string_view ticket :
       {TICKET_WITH_A_TOKEN_PREFIX, UPPERCASE_TICKET, SHORT_TICKET,
        LONG_TICKET}) {
    SCOPED_TRACE(ticket);
    EXPECT_FALSE(isWellFormedGameTicket(ticket));
  }
}

/**
 * Given an answer whose ticket is in uppercase
 * When the client reads it
 * Then it is refused
 */
TEST(GameTicketText, RefusesAnAnswerWithAMalformedTicket) {
  EXPECT_THROW(
      static_cast<void>(gameTicketGrantFromText(UPPERCASE_TICKET_ANSWER)),
      InvalidMasterMessageException);
}
