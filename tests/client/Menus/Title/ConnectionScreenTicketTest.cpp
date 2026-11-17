#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "ConnectionScreenConstants.hpp"
#include "ConnectionScreenFixtures.hpp"
#include "ConnectionScreenTestConstants.hpp"
#include "ConnectionScreenTicketFixture.hpp"
#include "GameTicket.hpp"
#include "GameTicketBytesTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::CONNECTING_STATUS;
using rtype::client::testing::JUST_UNDER_THE_TICKET_WAIT;
using rtype::client::testing::ONE_NANOSECOND;
using rtype::client::testing::SIGNED_IN_TOKEN;
using rtype::network::GUEST_TICKET;
using rtype::network::testing::SOME_TICKET;

/**
 * Given a player signed in to the master
 * When Enter starts an attempt
 * Then the screen asks the master a ticket with the token of the session,
 * sends no request yet, and says it is connecting
 */
TEST_F(ConnectionScreenTicketFixture, AnAttemptWaitsForItsTicket) {
  confirm();

  EXPECT_EQ(tickets().askedTokens(),
            std::vector<std::string>{std::string{SIGNED_IN_TOKEN}});
  EXPECT_TRUE(connectAttemptsOf(connection()).empty());
  EXPECT_NE(writingOf(canvas(), CONNECTING_STATUS), nullptr);
}

/**
 * Given an attempt of a signed-in player waiting for its ticket
 * When the master gives one, and a frame goes by
 * Then the attempt starts, carrying that ticket
 */
TEST_F(ConnectionScreenTicketFixture, AnAttemptCarriesTheTicketOfTheMaster) {
  confirm();
  tickets().answerLastAsk(SOME_TICKET);

  wait(ONE_FRAME);

  ASSERT_EQ(connectAttemptsOf(connection()).size(), 1U);
  EXPECT_EQ(connectAttemptsOf(connection()).front().ticket, SOME_TICKET);
}

/**
 * Given an attempt of a signed-in player waiting for its ticket
 * When the master does not answer: 2 s minus a nanosecond, then that
 * nanosecond
 * Then nothing starts before, and the attempt starts as a guest at 2 s
 */
TEST_F(ConnectionScreenTicketFixture, ASlowMasterLetsThePlayerJoinAsAGuest) {
  confirm();

  wait(JUST_UNDER_THE_TICKET_WAIT);
  EXPECT_TRUE(connectAttemptsOf(connection()).empty());

  wait(ONE_NANOSECOND);
  ASSERT_EQ(connectAttemptsOf(connection()).size(), 1U);
  EXPECT_EQ(connectAttemptsOf(connection()).front().ticket, GUEST_TICKET);
}

/**
 * Given an attempt of a signed-in player waiting for its ticket
 * When the master refuses it, and a frame goes by
 * Then the attempt starts as a guest
 */
TEST_F(ConnectionScreenTicketFixture, ARefusedTicketLetsThePlayerJoinAsAGuest) {
  confirm();
  tickets().answerLastAsk(std::nullopt);

  wait(ONE_FRAME);

  ASSERT_EQ(connectAttemptsOf(connection()).size(), 1U);
  EXPECT_EQ(connectAttemptsOf(connection()).front().ticket, GUEST_TICKET);
}

/**
 * Given a player signed out
 * When Enter starts an attempt
 * Then it starts at once as a guest, and the master is asked nothing
 */
TEST_F(ConnectionScreenTicketFixture, AGuestJoinsAtOnce) {
  account().signOut();

  confirm();

  ASSERT_EQ(connectAttemptsOf(connection()).size(), 1U);
  EXPECT_EQ(connectAttemptsOf(connection()).front().ticket, GUEST_TICKET);
  EXPECT_TRUE(tickets().askedTokens().empty());
}
