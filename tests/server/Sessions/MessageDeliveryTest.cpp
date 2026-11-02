#include <gtest/gtest.h>
#include <functional>
#include <utility>
#include <vector>
#include "GuaranteedMessageTestConstants.hpp"
#include "MessageDelivery.hpp"
#include "MessageDeliveryFixture.hpp"
#include "MessageDeliveryFixtures.hpp"
#include "MessageDeliveryTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "SequenceOrder.hpp"
#include "SessionsFixtures.hpp"

namespace rtype::server::testing {

/**
 * Given two players
 * When a message is sent once and a whole lifetime of ticks ends
 * Then each player got it once, in a datagram that says it holds no
 * guaranteed message
 */
TEST_F(MessageDeliveryFixture, SendsAMessageOnceToEveryPlayer) {
  sendOnce(playerLeftMessage());
  endTicks(TICKS_OF_A_LIFETIME);

  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    ASSERT_EQ(sentTo(player).size(), 1U);
    EXPECT_EQ(headerOf(sentTo(player).front()).flags, NO_FLAG);
  }
}

/**
 * Given two players
 * When a message is guaranteed
 * Then each player gets it at once, in a datagram that says it holds
 * guaranteed messages
 */
TEST_F(MessageDeliveryFixture, GuaranteesAMessageToEveryPlayerAtOnce) {
  guarantee(playerLeftMessage());

  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    ASSERT_EQ(sentTo(player).size(), 1U);
    EXPECT_EQ(headerOf(sentTo(player).front()).flags, PROTOCOL_GUARANTEED_FLAG);
  }
}

/**
 * Given a message guaranteed to two players who acknowledge nothing
 * When one tick less than TICKS_BEFORE_A_RESEND ends
 * Then it was not sent again
 */
TEST_F(MessageDeliveryFixture, DoesNotSendAgainBeforeTheResendDelay) {
  guarantee(playerLeftMessage());

  endTicks(TICKS_BEFORE_A_RESEND - 1);

  EXPECT_EQ(sentTo(FIRST_PLAYER).size(), 1U);
}

/**
 * Given a message guaranteed to two players who acknowledge nothing
 * When TICKS_BEFORE_A_RESEND ticks end
 * Then it was sent again, in a newer datagram
 */
TEST_F(MessageDeliveryFixture, SendsAgainAtTheResendDelayInANewerDatagram) {
  guarantee(playerLeftMessage());

  endTicks(TICKS_BEFORE_A_RESEND);

  const std::vector<network::OutgoingDatagram> received = sentTo(FIRST_PLAYER);
  ASSERT_EQ(received.size(), 2U);
  EXPECT_TRUE(network::isNewerSequence(headerOf(received.back()).sequence,
                                       headerOf(received.front()).sequence));
}

/**
 * Given a message guaranteed to two players
 * When the first acknowledges the datagram it got, and a lifetime of ticks
 * ends
 * Then the first got it once, while the second, who acknowledged nothing, got
 * it SENDS_IN_A_LIFETIME times
 */
TEST_F(MessageDeliveryFixture, StopsSendingOnceThePlayerAcknowledgedIt) {
  guarantee(playerLeftMessage());

  acknowledgeFrom(FIRST_PLAYER,
                  headerOf(sentTo(FIRST_PLAYER).front()).sequence);
  endTicks(TICKS_OF_A_LIFETIME);

  EXPECT_EQ(sentTo(FIRST_PLAYER).size(), 1U);
  EXPECT_EQ(sentTo(SECOND_PLAYER).size(), SENDS_IN_A_LIFETIME);
}

/**
 * Given a message guaranteed to a player who acknowledges nothing
 * When a lifetime of ticks ends
 * Then it went out SENDS_IN_A_LIFETIME times
 */
TEST_F(MessageDeliveryFixture, SendsAtMostNineTimesInALifetime) {
  guarantee(playerLeftMessage());

  endTicks(TICKS_OF_A_LIFETIME);

  EXPECT_EQ(sentTo(FIRST_PLAYER).size(), SENDS_IN_A_LIFETIME);
}

/**
 * Given a message guaranteed and never acknowledged, whose lifetime is over
 * When more ticks end
 * Then it is never sent again
 */
TEST_F(MessageDeliveryFixture, SendsNothingOnceTheLifetimeIsOver) {
  guarantee(playerLeftMessage());
  endTicks(TICKS_OF_A_LIFETIME);

  endTicks(TICKS_BEFORE_A_RESEND);

  EXPECT_EQ(sentTo(FIRST_PLAYER).size(), SENDS_IN_A_LIFETIME);
}

/**
 * Given two messages guaranteed in the same tick, each sent at once
 * When TICKS_BEFORE_A_RESEND ticks end
 * Then both are sent again in one datagram
 */
TEST_F(MessageDeliveryFixture, GroupsTheMessagesDueAgainInOneDatagram) {
  guarantee(playerLeftMessage());
  guarantee(entityDestroyedMessage());

  endTicks(TICKS_BEFORE_A_RESEND);

  const std::vector<network::OutgoingDatagram> received = sentTo(FIRST_PLAYER);
  ASSERT_EQ(received.size(), 3U);
  EXPECT_EQ(messageCountIn(received.back()), 2U);
}

/**
 * Given a message guaranteed to two players
 * When a third player joins, and TICKS_BEFORE_A_RESEND ticks end
 * Then the third player gets nothing
 */
TEST_F(MessageDeliveryFixture, GivesNothingToAPlayerWhoJoinsLater) {
  guarantee(playerLeftMessage());

  addPlayer(roster(), LATE_PLAYER);
  endTicks(TICKS_BEFORE_A_RESEND);

  EXPECT_TRUE(sentTo(LATE_PLAYER).empty());
}

/**
 * Given a roster without player
 * When a message is guaranteed and a tick ends
 * Then nothing is sent
 */
TEST(MessageDelivery, SendsNothingWithoutAPlayer) {
  PlayerRoster roster;
  std::vector<network::OutgoingDatagram> sent;
  const std::function<void(network::OutgoingDatagram)> send =
      [&sent](network::OutgoingDatagram datagram) {
        sent.push_back(std::move(datagram));
      };

  guaranteeToEveryPlayer(roster, send, playerLeftMessage());
  resendGuaranteedMessages(roster, send);

  EXPECT_TRUE(sent.empty());
}

}  // namespace rtype::server::testing
