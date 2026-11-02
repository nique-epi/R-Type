#include <gtest/gtest.h>
#include "NetworkTestConstants.hpp"
#include "PacketHeader.hpp"
#include "ReceivedSequences.hpp"
#include "ReliabilityTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given sequences that accepted nothing
 * When they acknowledge into a header
 * Then the header is left as it was: no flag, nothing acknowledged
 */
TEST(ReceivedSequences, AcknowledgesNothingBeforeTheFirstDatagram) {
  const ReceivedSequences sequences;
  PacketHeader header;

  sequences.acknowledgeIn(header);

  EXPECT_EQ(header, PacketHeader{});
}

/**
 * Given sequences that accepted nothing
 * When the datagram FIRST_RECEIVED arrives
 * Then it is accepted and is the newest
 */
TEST(ReceivedSequences, AcceptsTheFirstDatagramWhateverItsSequence) {
  ReceivedSequences sequences;

  const bool accepted = sequences.accept(FIRST_RECEIVED);

  EXPECT_TRUE(accepted);
  EXPECT_EQ(sequences.newest(), FIRST_RECEIVED);
}

/**
 * Given sequences that accepted FIRST_RECEIVED
 * When they acknowledge into a header
 * Then it carries the flag that acknowledges, FIRST_RECEIVED, and no earlier
 * datagram
 */
TEST(ReceivedSequences, AcknowledgesTheNewestDatagram) {
  ReceivedSequences sequences;
  ASSERT_TRUE(sequences.accept(FIRST_RECEIVED));
  PacketHeader header;

  sequences.acknowledgeIn(header);

  EXPECT_EQ(header.flags, PROTOCOL_ACKNOWLEDGES_FLAG);
  EXPECT_EQ(header.acknowledgedSequence, FIRST_RECEIVED);
  EXPECT_EQ(header.acknowledgementBits, NO_BIT);
}

/**
 * Given sequences that accepted 10, 11 and 13, 12 being lost
 * When they acknowledge into a header
 * Then 13 is acknowledged, bit 0 (12) is clear, bits 1 (11) and 2 (10) are set
 */
TEST(ReceivedSequences, SetsTheBitOfEachEarlierDatagramAccepted) {
  ReceivedSequences sequences;
  ASSERT_TRUE(sequences.accept(OLDEST_RECEIVED));
  ASSERT_TRUE(sequences.accept(RECEIVED_BEFORE_THE_LOSS));
  ASSERT_TRUE(sequences.accept(NEWEST_AFTER_THE_LOSS));
  PacketHeader header;

  sequences.acknowledgeIn(header);

  EXPECT_EQ(header.acknowledgedSequence, NEWEST_AFTER_THE_LOSS);
  EXPECT_EQ(header.acknowledgementBits, BITS_OF_ELEVEN_AND_TEN);
}

/**
 * Given sequences whose newest is 13
 * When 13 arrives again
 * Then it is refused, and the acknowledgement does not change
 */
TEST(ReceivedSequences, RefusesADuplicate) {
  ReceivedSequences sequences;
  ASSERT_TRUE(sequences.accept(NEWEST_AFTER_THE_LOSS));
  PacketHeader before;
  sequences.acknowledgeIn(before);

  const bool accepted = sequences.accept(NEWEST_AFTER_THE_LOSS);

  PacketHeader after;
  sequences.acknowledgeIn(after);
  EXPECT_FALSE(accepted);
  EXPECT_EQ(after, before);
}

/**
 * Given sequences whose newest is 13
 * When 12 arrives, late
 * Then it is refused, and the acknowledgement does not change: 12 is never
 * acknowledged, so its sender sends its guaranteed messages again
 */
TEST(ReceivedSequences, RefusesALateDatagram) {
  ReceivedSequences sequences;
  ASSERT_TRUE(sequences.accept(NEWEST_AFTER_THE_LOSS));
  PacketHeader before;
  sequences.acknowledgeIn(before);

  const bool accepted = sequences.accept(LOST_SEQUENCE);

  PacketHeader after;
  sequences.acknowledgeIn(after);
  EXPECT_FALSE(accepted);
  EXPECT_EQ(after, before);
}

/**
 * Given sequences that accepted 0
 * When 32 arrives
 * Then 0 is still acknowledged, by bit 31 alone
 */
TEST(ReceivedSequences, KeepsTheDatagramThirtyTwoBeforeTheNewest) {
  ReceivedSequences sequences;
  ASSERT_TRUE(sequences.accept(ZERO_SEQUENCE));
  ASSERT_TRUE(sequences.accept(THIRTY_TWO_LATER));
  PacketHeader header;

  sequences.acknowledgeIn(header);

  EXPECT_EQ(header.acknowledgementBits, ONLY_BIT_THIRTY_ONE);
}

/**
 * Given sequences that accepted 0
 * When 33 arrives
 * Then no bit names 0 any more
 */
TEST(ReceivedSequences, ForgetsTheDatagramThirtyThreeBeforeTheNewest) {
  ReceivedSequences sequences;
  ASSERT_TRUE(sequences.accept(ZERO_SEQUENCE));
  ASSERT_TRUE(sequences.accept(THIRTY_THREE_LATER));
  PacketHeader header;

  sequences.acknowledgeIn(header);

  EXPECT_EQ(header.acknowledgementBits, NO_BIT);
}

/**
 * Given sequences whose newest is 65535
 * When 0 arrives
 * Then it is accepted, and 65535 is acknowledged by bit 0
 */
TEST(ReceivedSequences, AcknowledgesAcrossTheWrap) {
  ReceivedSequences sequences;
  ASSERT_TRUE(sequences.accept(LAST_SEQUENCE));

  const bool accepted = sequences.accept(ZERO_SEQUENCE);

  PacketHeader header;
  sequences.acknowledgeIn(header);
  EXPECT_TRUE(accepted);
  EXPECT_EQ(header.acknowledgedSequence, ZERO_SEQUENCE);
  EXPECT_EQ(header.acknowledgementBits, ONLY_BIT_ZERO);
}

/**
 * Given sequences started at FIRST_RECEIVED, as the client starts them at
 * its acceptance
 * When FIRST_RECEIVED arrives again
 * Then it is refused: it counts as accepted already
 */
TEST(ReceivedSequences, StartsAtTheDatagramItIsGiven) {
  ReceivedSequences sequences = ReceivedSequences::startingAt(FIRST_RECEIVED);

  EXPECT_FALSE(sequences.accept(FIRST_RECEIVED));
}

/**
 * Given a header that already says it holds guaranteed messages
 * When sequences that accepted a datagram acknowledge into it
 * Then it carries both flags
 */
TEST(ReceivedSequences, KeepsTheFlagsTheHeaderAlreadyHas) {
  ReceivedSequences sequences;
  ASSERT_TRUE(sequences.accept(FIRST_RECEIVED));
  PacketHeader header;
  header.flags = PROTOCOL_GUARANTEED_FLAG;

  sequences.acknowledgeIn(header);

  EXPECT_EQ(header.flags, PROTOCOL_BOTH_FLAGS);
}

}  // namespace rtype::network::testing
