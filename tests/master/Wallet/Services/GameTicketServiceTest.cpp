#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <optional>
#include "GameTicketGrant.hpp"
#include "GameTicketServiceFixture.hpp"
#include "GameTicketServiceTestConstants.hpp"
#include "GameTicketState.hpp"
#include "GameTicketText.hpp"
#include "MasterMessageText.hpp"
#include "WalletServiceTestConstants.hpp"

using rtype::master::gameTicketFingerprintOf;
using rtype::network::GameTicketGrant;
using rtype::network::GameTicketState;

/**
 * Given the account ACE
 * When the master gives it a ticket
 * Then the ticket is well formed and expires 6 hours later, and the master
 * keeps its fingerprint, never the ticket
 */
TEST_F(GameTicketServiceFixture, KeepsTheFingerprintOfATicketOnly) {
  const GameTicketGrant grant =
      tickets().issue(ace(), WALLET_TEST_DATE).value_or(GameTicketGrant{});

  EXPECT_TRUE(rtype::network::isWellFormedGameTicket(grant.ticket));
  EXPECT_EQ(grant.expiresAt, WALLET_TEST_DATE + SIX_HOURS);
  EXPECT_EQ(rowsKeyedBy(gameTicketFingerprintOf(grant.ticket)), 1);
  EXPECT_EQ(rowsKeyedBy(grant.ticket), 0);
}

/**
 * Given a ticket ACE was just given
 * When ACE reads it by its fingerprint
 * Then it expires 6 hours later and no result used it yet
 */
TEST_F(GameTicketServiceFixture, ReadsANewTicketAsUnused) {
  const GameTicketGrant grant =
      tickets().issue(ace(), WALLET_TEST_DATE).value_or(GameTicketGrant{});

  const GameTicketState unused{.expiresAt = WALLET_TEST_DATE + SIX_HOURS,
                               .used = false};

  EXPECT_EQ(tickets().stateOf(ace(), gameTicketFingerprintOf(grant.ticket)),
            std::optional{unused});
}

/**
 * Given a ticket ACE was given
 * When another account reads it by its fingerprint
 * Then the master finds none
 */
TEST_F(GameTicketServiceFixture, ATicketIsReadByItsAccountOnly) {
  const GameTicketGrant grant =
      tickets().issue(ace(), WALLET_TEST_DATE).value_or(GameTicketGrant{});

  EXPECT_EQ(
      tickets().stateOf(UNKNOWN_USER_ID, gameTicketFingerprintOf(grant.ticket)),
      std::nullopt);
}

/**
 * Given ACE was given 30 tickets within an hour
 * When it asks one more in that hour, then once the first is an hour old
 * Then the master gives none, then one again
 */
TEST_F(GameTicketServiceFixture, GivesThirtyTicketsAnHour) {
  for (std::size_t given = 0; given < TICKETS_OF_AN_HOUR; ++given) {
    ASSERT_TRUE(tickets().issue(ace(), WALLET_TEST_DATE).has_value());
  }

  EXPECT_EQ(tickets().issue(ace(), WALLET_TEST_DATE), std::nullopt);
  EXPECT_TRUE(
      tickets().issue(ace(), WALLET_TEST_DATE + JUST_OVER_AN_HOUR).has_value());
}

/**
 * Given a ticket ACE was given
 * When the master gives another once the first expired
 * Then the first is forgotten
 */
TEST_F(GameTicketServiceFixture, ForgetsTheExpiredTickets) {
  const GameTicketGrant first =
      tickets().issue(ace(), WALLET_TEST_DATE).value_or(GameTicketGrant{});

  static_cast<void>(tickets().issue(
      ace(), WALLET_TEST_DATE + SIX_HOURS + std::chrono::seconds{1}));

  EXPECT_EQ(rowsKeyedBy(gameTicketFingerprintOf(first.ticket)), 0);
}
