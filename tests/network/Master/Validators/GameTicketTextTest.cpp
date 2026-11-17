#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "FinishedGame.hpp"
#include "GameTicketGrant.hpp"
#include "GameTicketTextTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "NetworkException.hpp"

using rtype::network::FinishedGame;
using rtype::network::finishedGameFromText;
using rtype::network::finishedGameText;
using rtype::network::GameTicketGrant;
using rtype::network::gameTicketGrantFromText;
using rtype::network::gameTicketGrantText;
using rtype::network::InvalidMasterMessageException;
using rtype::network::isWellFormedGameTicket;
using rtype::network::testing::finishedGameRefusal;
using rtype::network::testing::finishedGameTextWith;
using rtype::network::testing::parisFinishedGame;
using rtype::network::testing::quoted;
using ::testing::HasSubstr;

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

/**
 * Given a finished game whose second pilot joined with a ticket
 * When it is written as a server reports it, then read back
 * Then the pilot carries the same ticket, and the first, a guest, none
 */
TEST(GameTicketText, AResultCarriesTheTicketOfAPilot) {
  FinishedGame game = parisFinishedGame();
  game.players.at(1).ticket = std::string{WELL_FORMED_TICKET};

  const FinishedGame read = finishedGameFromText(finishedGameText(game));

  EXPECT_EQ(read.players.at(1).ticket,
            std::optional{std::string{WELL_FORMED_TICKET}});
  EXPECT_EQ(read.players.at(0).ticket, std::nullopt);
}

/**
 * Given a finished game whose second pilot names a ticket in uppercase
 * When the master reads it
 * Then it refuses it, naming the ticket of that pilot
 */
TEST(GameTicketText, RefusesAResultWithAMalformedTicket) {
  EXPECT_THAT(finishedGameRefusal(finishedGameTextWith(
                  SECOND_PILOT_TICKET_POINTER,
                  nlohmann::json(std::string{UPPERCASE_TICKET}))),
              HasSubstr(quoted(SECOND_PILOT_TICKET_POINTER)));
}
