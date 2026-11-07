#include <gtest/gtest.h>
#include "GuaranteedDelivery.hpp"
#include "NetworkTestConstants.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "ReliabilityFixtures.hpp"
#include "ReliabilityTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given a header naming FIRST_SEND with every bit set, but not the flag that
 * acknowledges
 * When it is asked about FIRST_SEND
 * Then it acknowledges nothing
 */
TEST(GuaranteedDelivery, AcknowledgesNothingWithoutTheFlag) {
  PacketHeader header = acknowledgingHeader(FIRST_SEND, EVERY_BIT);
  header.flags = 0;

  EXPECT_FALSE(acknowledges(header, FIRST_SEND));
}

/**
 * Given a header acknowledging FIRST_SEND
 * When it is asked about FIRST_SEND
 * Then it acknowledges it
 */
TEST(GuaranteedDelivery, AcknowledgesTheNewestSequence) {
  EXPECT_TRUE(
      acknowledges(acknowledgingHeader(FIRST_SEND, NO_BIT), FIRST_SEND));
}

/**
 * Given a header acknowledging 13 whose bits name 11 and 10
 * When it is asked about 11
 * Then it acknowledges it
 */
TEST(GuaranteedDelivery, AcknowledgesAnEarlierSequenceWhoseBitIsSet) {
  EXPECT_TRUE(acknowledges(
      acknowledgingHeader(NEWEST_AFTER_THE_LOSS, BITS_OF_ELEVEN_AND_TEN),
      RECEIVED_BEFORE_THE_LOSS));
}

/**
 * Given a header acknowledging 13 whose bits name 11 and 10
 * When it is asked about 12
 * Then it does not acknowledge it
 */
TEST(GuaranteedDelivery, DoesNotAcknowledgeAnEarlierSequenceWhoseBitIsClear) {
  EXPECT_FALSE(acknowledges(
      acknowledgingHeader(NEWEST_AFTER_THE_LOSS, BITS_OF_ELEVEN_AND_TEN),
      LOST_SEQUENCE));
}

/**
 * Given a header acknowledging 32 whose bit 31 alone is set
 * When it is asked about 0
 * Then it acknowledges it
 */
TEST(GuaranteedDelivery, AcknowledgesTheSequenceThirtyTwoBefore) {
  EXPECT_TRUE(
      acknowledges(acknowledgingHeader(THIRTY_TWO_LATER, ONLY_BIT_THIRTY_ONE),
                   ZERO_SEQUENCE));
}

/**
 * Given a header acknowledging 33 with every bit set
 * When it is asked about 0
 * Then it does not acknowledge it: no bit names a sequence 33 before
 */
TEST(GuaranteedDelivery, NeverAcknowledgesTheSequenceThirtyThreeBefore) {
  EXPECT_FALSE(acknowledges(acknowledgingHeader(THIRTY_THREE_LATER, EVERY_BIT),
                            ZERO_SEQUENCE));
}

/**
 * Given a header acknowledging FIRST_SEND with every bit set
 * When it is asked about the sequence just after
 * Then it does not acknowledge it
 */
TEST(GuaranteedDelivery, NeverAcknowledgesASequenceAfterTheNewest) {
  EXPECT_FALSE(acknowledges(acknowledgingHeader(FIRST_SEND, EVERY_BIT),
                            SEND_AFTER_THE_FIRST));
}

/**
 * Given a header acknowledging 0 whose bit 0 is set
 * When it is asked about 65535
 * Then it acknowledges it
 */
TEST(GuaranteedDelivery, AcknowledgesAcrossTheWrap) {
  EXPECT_TRUE(acknowledges(acknowledgingHeader(ZERO_SEQUENCE, ONLY_BIT_ZERO),
                           LAST_SEQUENCE));
}

/**
 * Given a header whose flags say it holds guaranteed messages
 * When it is asked
 * Then it holds guaranteed messages
 */
TEST(GuaranteedDelivery, TellsADatagramThatHoldsGuaranteedMessages) {
  PacketHeader header;
  header.flags = PROTOCOL_GUARANTEED_FLAG;

  EXPECT_TRUE(holdsGuaranteedMessages(header));
}

/**
 * Given a header whose flags only say it acknowledges
 * When it is asked
 * Then it holds no guaranteed message
 */
TEST(GuaranteedDelivery, TellsADatagramThatOnlyAcknowledges) {
  PacketHeader header;
  header.flags = PROTOCOL_ACKNOWLEDGES_FLAG;

  EXPECT_FALSE(holdsGuaranteedMessages(header));
}

/**
 * Given a header that already acknowledges
 * When it is marked as holding guaranteed messages
 * Then it carries both flags
 */
TEST(GuaranteedDelivery, MarksADatagramThatHoldsGuaranteedMessages) {
  PacketHeader header;
  header.flags = PROTOCOL_ACKNOWLEDGES_FLAG;

  markGuaranteed(header);

  EXPECT_EQ(header.flags, PROTOCOL_BOTH_FLAGS);
}

/**
 * Given the protocol page
 * When the constants of guaranteed delivery are read
 * Then they are its values: flags 0x01 and 0x02, 32 bits, 100 ms, 1 s, 40 ms,
 * version 11
 */
TEST(GuaranteedDeliveryConstants, AreThoseOfTheProtocolPage) {
  EXPECT_EQ(ACKNOWLEDGES_FLAG, PROTOCOL_ACKNOWLEDGES_FLAG);
  EXPECT_EQ(GUARANTEED_FLAG, PROTOCOL_GUARANTEED_FLAG);
  EXPECT_EQ(ACKNOWLEDGEMENT_BIT_COUNT, PROTOCOL_ACKNOWLEDGEMENT_BIT_COUNT);
  EXPECT_EQ(GUARANTEED_RESEND_DELAY, PROTOCOL_RESEND_DELAY);
  EXPECT_EQ(GUARANTEED_MESSAGE_LIFETIME, PROTOCOL_MESSAGE_LIFETIME);
  EXPECT_EQ(ACKNOWLEDGEMENT_DELAY, PROTOCOL_ACKNOWLEDGEMENT_DELAY);
  EXPECT_EQ(PROTOCOL_VERSION, PROTOCOL_PAGE_VERSION);
}

}  // namespace rtype::network::testing
