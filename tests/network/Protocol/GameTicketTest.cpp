#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <string_view>
#include "GameTicket.hpp"
#include "GameTicketBytesTestConstants.hpp"
#include "GameTicketTestConstants.hpp"

using rtype::network::GameTicket;
using rtype::network::gameTicketOfText;
using rtype::network::gameTicketText;
using rtype::network::GUEST_TICKET;
using rtype::network::isGuestTicket;
using rtype::network::testing::SOME_TICKET;
using rtype::network::testing::SOME_TICKET_TEXT;

/**
 * Given a ticket of bytes 0x20 to 0x3F
 * When it is written as the master writes it
 * Then it is `rtt_` and its bytes in lowercase hexadecimal, in their order
 */
TEST(GameTicket, IsWrittenAsTheMasterWritesIt) {
  EXPECT_EQ(gameTicketText(SOME_TICKET),
            std::optional{std::string{SOME_TICKET_TEXT}});
}

/**
 * Given the text of a ticket the master gave
 * When the client reads it
 * Then it reads the same bytes
 */
TEST(GameTicket, ReadsTheTextOfATicket) {
  EXPECT_EQ(gameTicketOfText(SOME_TICKET_TEXT), std::optional{SOME_TICKET});
}

/**
 * Given the ticket of a guest
 * When it is written
 * Then it has no text: a guest has no ticket to report
 */
TEST(GameTicket, AGuestHasNoText) {
  EXPECT_TRUE(isGuestTicket(GUEST_TICKET));
  EXPECT_EQ(gameTicketText(GUEST_TICKET), std::nullopt);
}

/**
 * Given texts shaped almost like a ticket: the prefix of a session token,
 * uppercase digits, a digit short, a digit too many, a letter past f
 * When the client reads each
 * Then none is a ticket
 */
TEST(GameTicket, RefusesATextShapedAlmostRight) {
  for (const std::string_view text :
       {TOKEN_PREFIXED_TEXT, UPPERCASE_TEXT, SHORT_TEXT, LONG_TEXT,
        LETTER_PAST_F_TEXT}) {
    SCOPED_TRACE(text);
    EXPECT_EQ(gameTicketOfText(text), std::optional<GameTicket>{});
  }
}
