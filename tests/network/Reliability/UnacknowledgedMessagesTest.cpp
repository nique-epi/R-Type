#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "EncodedMessage.hpp"
#include "NetworkException.hpp"
#include "NetworkTestConstants.hpp"
#include "OrderedMessage.hpp"
#include "PacketHeader.hpp"
#include "ProtocolFixtures.hpp"
#include "ProtocolTestConstants.hpp"
#include "ReliabilityFixtures.hpp"
#include "ReliabilityTestConstants.hpp"
#include "TimeTestConstants.hpp"
#include "UnacknowledgedMessages.hpp"
#include "UnacknowledgedMessagesFixture.hpp"

namespace rtype::network::testing {

/**
 * Given a message just given
 * When the due messages are taken
 * Then it is among them, alone
 */
TEST(UnacknowledgedMessages, AMessageIsDueAsSoonAsItIsGiven) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));

  const std::vector<EncodedMessage> due = messages.takeDue(FIRST_SEND);

  ASSERT_EQ(due.size(), 1U);
  EXPECT_EQ(numberOf(due.front()), FIRST_MESSAGE);
}

/**
 * Given a message sent in FIRST_SEND
 * When one nanosecond less than the resend delay passes
 * Then it is not due
 */
TEST(UnacknowledgedMessages, AMessageSentIsNotDueBeforeTheResendDelay) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));
  ASSERT_EQ(messages.takeDue(FIRST_SEND).size(), 1U);

  messages.advance(PROTOCOL_RESEND_DELAY - ONE_NANOSECOND);

  EXPECT_FALSE(messages.hasDue());
}

/**
 * Given a message sent in FIRST_SEND
 * When the resend delay passes
 * Then it is due again
 */
TEST(UnacknowledgedMessages, AMessageSentIsDueAgainAtTheResendDelay) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));
  ASSERT_EQ(messages.takeDue(FIRST_SEND).size(), 1U);

  messages.advance(PROTOCOL_RESEND_DELAY);

  EXPECT_TRUE(messages.hasDue());
}

/**
 * Given a message sent in FIRST_SEND
 * When a header acknowledging FIRST_SEND arrives
 * Then the message is dropped
 */
TEST(UnacknowledgedMessages, AnAcknowledgedMessageIsDropped) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));
  ASSERT_EQ(messages.takeDue(FIRST_SEND).size(), 1U);

  messages.acknowledge(acknowledgingHeader(FIRST_SEND, NO_BIT));

  EXPECT_EQ(messages.count(), 0U);
}

/**
 * Given a message sent in FIRST_SEND, then again in SECOND_SEND
 * When a header acknowledging FIRST_SEND alone arrives
 * Then the message is dropped: its first copy arrived
 */
TEST(UnacknowledgedMessages, AnAcknowledgementOfAnEarlierSendCounts) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));
  ASSERT_EQ(messages.takeDue(FIRST_SEND).size(), 1U);
  messages.advance(PROTOCOL_RESEND_DELAY);
  ASSERT_EQ(messages.takeDue(SECOND_SEND).size(), 1U);

  messages.acknowledge(acknowledgingHeader(FIRST_SEND, NO_BIT));

  EXPECT_EQ(messages.count(), 0U);
}

/**
 * Given a message sent in FIRST_SEND
 * When a header naming FIRST_SEND without the flag that acknowledges arrives
 * Then the message is kept
 */
TEST(UnacknowledgedMessages, AHeaderThatAcknowledgesNothingDropsNothing) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));
  ASSERT_EQ(messages.takeDue(FIRST_SEND).size(), 1U);
  PacketHeader header = acknowledgingHeader(FIRST_SEND, NO_BIT);
  header.flags = 0;

  messages.acknowledge(header);

  EXPECT_EQ(messages.count(), 1U);
}

/**
 * Given a message never acknowledged
 * When one nanosecond less than its lifetime passes
 * Then it is still kept
 */
TEST(UnacknowledgedMessages, AMessageIsKeptUntilTheEndOfItsLifetime) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));

  messages.advance(PROTOCOL_MESSAGE_LIFETIME - ONE_NANOSECOND);

  EXPECT_EQ(messages.count(), 1U);
}

/**
 * Given a message never acknowledged
 * When its lifetime passes
 * Then it is dropped
 */
TEST(UnacknowledgedMessages, AMessageIsDroppedAtTheEndOfItsLifetime) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));

  messages.advance(PROTOCOL_MESSAGE_LIFETIME);

  EXPECT_EQ(messages.count(), 0U);
}

/**
 * Given two messages due
 * When the due messages are taken
 * Then both come in one datagram, the oldest first
 */
TEST(UnacknowledgedMessages, TakesTheDueMessagesOldestFirst) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));
  messages.add(numberedMessage(SECOND_MESSAGE));

  const std::vector<EncodedMessage> due = messages.takeDue(FIRST_SEND);

  ASSERT_EQ(due.size(), 2U);
  EXPECT_EQ(numberOf(due.front()), FIRST_MESSAGE);
  EXPECT_EQ(numberOf(due.back()), SECOND_MESSAGE);
}

/**
 * Given two messages due that cannot share a datagram
 * When the due messages are taken
 * Then the first datagram holds one, and the other is still due for the next
 */
TEST(UnacknowledgedMessages, TakesOnlyTheMessagesThatFitOneDatagram) {
  UnacknowledgedMessages messages;
  messages.add(messageOfSize(HALF_DATAGRAM_PAYLOAD_SIZE));
  messages.add(messageOfSize(HALF_DATAGRAM_PAYLOAD_SIZE));

  const std::vector<EncodedMessage> first = messages.takeDue(FIRST_SEND);

  EXPECT_EQ(first.size(), 1U);
  EXPECT_EQ(messages.takeDue(SECOND_SEND).size(), 1U);
}

/**
 * Given nothing due
 * When the due messages are taken
 * Then none is
 */
TEST(UnacknowledgedMessages, TakesNothingWhenNothingIsDue) {
  UnacknowledgedMessages messages;

  EXPECT_TRUE(messages.takeDue(FIRST_SEND).empty());
}

/**
 * Given the largest payload one message carries alone
 * When it is given
 * Then it is kept
 */
TEST(UnacknowledgedMessages, KeepsTheLargestMessageADatagramHolds) {
  UnacknowledgedMessages messages;

  messages.add(messageOfSize(LARGEST_CARRIED_PAYLOAD_SIZE));

  EXPECT_EQ(messages.count(), 1U);
}

/**
 * Given a payload one byte larger than a datagram holds alone
 * When it is given
 * Then it is refused with DatagramTooLargeException, and nothing is kept
 */
TEST(UnacknowledgedMessages, RefusesAMessageTooLargeForADatagram) {
  UnacknowledgedMessages messages;

  EXPECT_THROW(messages.add(messageOfSize(PAYLOAD_ONE_BYTE_TOO_LARGE)),
               DatagramTooLargeException);
  EXPECT_EQ(messages.count(), 0U);
}

/**
 * Given two messages given in turn
 * When the due messages are taken
 * Then they are two envelopes numbered 1 and 2 in order, each carrying its
 * message and keeping from number 1
 */
TEST(UnacknowledgedMessages, NumbersTheMessagesFromOneInTheOrderTheyAreGiven) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));
  messages.add(numberedMessage(SECOND_MESSAGE));

  const std::vector<EncodedMessage> due = messages.takeDue(FIRST_SEND);

  ASSERT_EQ(due.size(), 2U);
  const OrderedMessage first = decodedEnvelope(due.at(0));
  const OrderedMessage second = decodedEnvelope(due.at(1));
  EXPECT_EQ(first.number, FIRST_NUMBER);
  EXPECT_EQ(second.number, SECOND_NUMBER);
  EXPECT_EQ(first.firstKept, FIRST_NUMBER);
  EXPECT_EQ(first.payload, numberedMessage(FIRST_MESSAGE).payload);
}

/**
 * Given a message of number 1 that grew old and one of number 2 that did not
 * When the due messages are taken
 * Then only number 2 is sent, keeping from number 2
 */
TEST(UnacknowledgedMessages, KeepsFromTheMessageAfterTheOneGivenUp) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));
  messages.advance(MOST_OF_A_LIFETIME);
  messages.add(numberedMessage(SECOND_MESSAGE));
  messages.advance(MOST_OF_A_LIFETIME);

  const std::vector<EncodedMessage> due = messages.takeDue(FIRST_SEND);

  ASSERT_EQ(due.size(), 1U);
  EXPECT_EQ(decodedEnvelope(due.front()).number, SECOND_NUMBER);
  EXPECT_EQ(decodedEnvelope(due.front()).firstKept, SECOND_NUMBER);
}

/**
 * Given a message of number 1 that was acknowledged and left
 * When the message of number 2 is given and taken
 * Then it keeps from number 1: nothing was given up
 */
TEST(UnacknowledgedMessages, KeepsFromTheStartWhenOnlyAnAcknowledgementLeft) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));
  ASSERT_EQ(messages.takeDue(FIRST_SEND).size(), 1U);
  messages.acknowledge(acknowledgingHeader(FIRST_SEND, NO_BIT));
  messages.add(numberedMessage(SECOND_MESSAGE));

  const std::vector<EncodedMessage> due =
      messages.takeDue(SEND_AFTER_THE_FIRST);

  ASSERT_EQ(due.size(), 1U);
  EXPECT_EQ(decodedEnvelope(due.front()).firstKept, FIRST_NUMBER);
}

/**
 * Given messages 1 and 2 sent, then 1 given up while 2 is still kept
 * When message 2 is due again
 * Then it is sent keeping from number 2, not from number 1 as the first time
 */
TEST(UnacknowledgedMessages, SendsTheCurrentFirstKeptOnEachResend) {
  UnacknowledgedMessages messages;
  messages.add(numberedMessage(FIRST_MESSAGE));
  messages.advance(MOST_OF_A_LIFETIME);
  messages.add(numberedMessage(SECOND_MESSAGE));
  ASSERT_EQ(messages.takeDue(FIRST_SEND).size(), 2U);
  messages.advance(HALF_A_LIFETIME);

  const std::vector<EncodedMessage> due =
      messages.takeDue(SEND_AFTER_THE_FIRST);

  ASSERT_EQ(due.size(), 1U);
  EXPECT_EQ(decodedEnvelope(due.front()).number, SECOND_NUMBER);
  EXPECT_EQ(decodedEnvelope(due.front()).firstKept, SECOND_NUMBER);
}

/**
 * Given 256 messages given, none acknowledged
 * When a 257th is given
 * Then the first is given up: 256 are kept
 */
TEST(UnacknowledgedMessages, GivesUpTheMessageTheWindowLeftBehind) {
  UnacknowledgedMessages messages;
  for (std::size_t given = 0; given < MESSAGES_KEPT_BY_THE_WINDOW; ++given) {
    messages.add(numberedMessage(FIRST_MESSAGE));
  }
  ASSERT_EQ(messages.count(), MESSAGES_KEPT_BY_THE_WINDOW);

  messages.add(numberedMessage(FIRST_MESSAGE));

  EXPECT_EQ(messages.count(), MESSAGES_KEPT_BY_THE_WINDOW);
}

/**
 * Given 255 messages given, none acknowledged
 * When a 256th is given
 * Then none is given up: 256 are kept
 */
TEST(UnacknowledgedMessages, KeepsEveryMessageOfAFullWindow) {
  UnacknowledgedMessages messages;
  for (std::size_t given = 0; given + 1 < MESSAGES_KEPT_BY_THE_WINDOW;
       ++given) {
    messages.add(numberedMessage(FIRST_MESSAGE));
  }

  messages.add(numberedMessage(FIRST_MESSAGE));

  EXPECT_EQ(messages.count(), MESSAGES_KEPT_BY_THE_WINDOW);
}

/**
 * Given a link that loses one datagram in five each way
 * When DEMONSTRATION_MESSAGE_COUNT guaranteed messages are given, one per
 * step, and the steps go on until the last one lived its lifetime
 * Then the receiver got every one of them
 */
TEST_F(UnacknowledgedMessagesFixture, EveryMessageArrivesThroughOneLossInFive) {
  runTheDemonstration();

  EXPECT_EQ(distinctMessagesDelivered(), DEMONSTRATION_MESSAGE_COUNT);
}

}  // namespace rtype::network::testing
