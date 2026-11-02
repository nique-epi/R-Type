#include <gtest/gtest.h>
#include "NetworkTestConstants.hpp"
#include "PacketHeader.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"
#include "SessionAcknowledgements.hpp"
#include "TimeTestConstants.hpp"

namespace rtype::client {

using testing::ACCEPTANCE_SEQUENCE;
using testing::FIRST_SEQUENCE;
using testing::guaranteedSessionHeader;
using testing::SECOND_SEQUENCE;
using testing::sessionHeader;

/**
 * Given acknowledgements of no session
 * When they are written into a header
 * Then the header is left as it was: no flag, nothing acknowledged
 */
TEST(SessionAcknowledgements, AcknowledgeNothingBeforeTheSession) {
  SessionAcknowledgements acknowledgements;
  network::PacketHeader header;

  acknowledgements.acknowledgeIn(header);

  EXPECT_EQ(header, network::PacketHeader{});
}

/**
 * Given a session opened by an acceptance of ACCEPTANCE_SEQUENCE
 * When the acknowledgements are written into a header
 * Then the header acknowledges the acceptance
 */
TEST(SessionAcknowledgements, AcknowledgeTheAcceptanceOnceOpened) {
  SessionAcknowledgements acknowledgements;
  acknowledgements.open(ACCEPTANCE_SEQUENCE);
  network::PacketHeader header;

  acknowledgements.acknowledgeIn(header);

  EXPECT_EQ(header.flags, PROTOCOL_ACKNOWLEDGES_FLAG);
  EXPECT_EQ(header.acknowledgedSequence, ACCEPTANCE_SEQUENCE);
}

/**
 * Given a session that accepted SECOND_SEQUENCE
 * When FIRST_SEQUENCE arrives, late
 * Then it is refused
 */
TEST(SessionAcknowledgements, RefuseADatagramOlderThanTheNewest) {
  SessionAcknowledgements acknowledgements;
  acknowledgements.open(ACCEPTANCE_SEQUENCE);
  ASSERT_TRUE(acknowledgements.accept(sessionHeader(SECOND_SEQUENCE)));

  EXPECT_FALSE(acknowledgements.accept(sessionHeader(FIRST_SEQUENCE)));
}

/**
 * Given a session that accepted a datagram holding guaranteed messages
 * When one nanosecond less than ACKNOWLEDGEMENT_DELAY passes, then that
 * nanosecond
 * Then the acknowledgement is not due, then due
 */
TEST(SessionAcknowledgements, AreDueTheAcknowledgementDelayAfterAGuarantee) {
  SessionAcknowledgements acknowledgements;
  acknowledgements.open(ACCEPTANCE_SEQUENCE);
  ASSERT_TRUE(acknowledgements.accept(guaranteedSessionHeader(FIRST_SEQUENCE)));

  acknowledgements.advance(PROTOCOL_ACKNOWLEDGEMENT_DELAY - ONE_NANOSECOND);
  const bool dueBefore = acknowledgements.isAcknowledgementDue();
  acknowledgements.advance(ONE_NANOSECOND);

  EXPECT_FALSE(dueBefore);
  EXPECT_TRUE(acknowledgements.isAcknowledgementDue());
}

/**
 * Given a session that accepted a datagram without guaranteed messages
 * When ACKNOWLEDGEMENT_DELAY passes
 * Then no acknowledgement is due: the next datagram will carry it
 */
TEST(SessionAcknowledgements, AreNotDueForADatagramWithoutGuarantee) {
  SessionAcknowledgements acknowledgements;
  acknowledgements.open(ACCEPTANCE_SEQUENCE);
  ASSERT_TRUE(acknowledgements.accept(sessionHeader(FIRST_SEQUENCE)));

  acknowledgements.advance(PROTOCOL_ACKNOWLEDGEMENT_DELAY);

  EXPECT_FALSE(acknowledgements.isAcknowledgementDue());
}

/**
 * Given a session that accepted SECOND_SEQUENCE
 * When FIRST_SEQUENCE arrives late, holding guaranteed messages, and
 * ACKNOWLEDGEMENT_DELAY passes
 * Then no acknowledgement is due: a refused datagram is never acknowledged
 */
TEST(SessionAcknowledgements, WaitForNoRefusedDatagram) {
  SessionAcknowledgements acknowledgements;
  acknowledgements.open(ACCEPTANCE_SEQUENCE);
  ASSERT_TRUE(acknowledgements.accept(sessionHeader(SECOND_SEQUENCE)));
  ASSERT_FALSE(
      acknowledgements.accept(guaranteedSessionHeader(FIRST_SEQUENCE)));

  acknowledgements.advance(PROTOCOL_ACKNOWLEDGEMENT_DELAY);

  EXPECT_FALSE(acknowledgements.isAcknowledgementDue());
}

/**
 * Given a session that accepted a datagram holding guaranteed messages
 * When a header carries the acknowledgement, then ACKNOWLEDGEMENT_DELAY passes
 * Then no acknowledgement is due any more
 */
TEST(SessionAcknowledgements, EndTheWaitWhenAHeaderCarriesThem) {
  SessionAcknowledgements acknowledgements;
  acknowledgements.open(ACCEPTANCE_SEQUENCE);
  ASSERT_TRUE(acknowledgements.accept(guaranteedSessionHeader(FIRST_SEQUENCE)));
  network::PacketHeader header;

  acknowledgements.acknowledgeIn(header);
  acknowledgements.advance(PROTOCOL_ACKNOWLEDGEMENT_DELAY);

  EXPECT_FALSE(acknowledgements.isAcknowledgementDue());
}

/**
 * Given a session that accepted a datagram holding guaranteed messages, and
 * one nanosecond less than ACKNOWLEDGEMENT_DELAY later a second one
 * When one more nanosecond passes
 * Then the acknowledgement is due: the second datagram did not start the wait
 * again
 */
TEST(SessionAcknowledgements, KeepTheWaitOfTheOldestGuarantee) {
  SessionAcknowledgements acknowledgements;
  acknowledgements.open(ACCEPTANCE_SEQUENCE);
  ASSERT_TRUE(acknowledgements.accept(guaranteedSessionHeader(FIRST_SEQUENCE)));
  acknowledgements.advance(PROTOCOL_ACKNOWLEDGEMENT_DELAY - ONE_NANOSECOND);
  ASSERT_TRUE(
      acknowledgements.accept(guaranteedSessionHeader(SECOND_SEQUENCE)));

  acknowledgements.advance(ONE_NANOSECOND);

  EXPECT_TRUE(acknowledgements.isAcknowledgementDue());
}

/**
 * Given a session that accepted a datagram
 * When the session closes
 * Then a header acknowledges nothing
 */
TEST(SessionAcknowledgements, ForgetTheSequencesWhenTheSessionCloses) {
  SessionAcknowledgements acknowledgements;
  acknowledgements.open(ACCEPTANCE_SEQUENCE);
  ASSERT_TRUE(acknowledgements.accept(sessionHeader(FIRST_SEQUENCE)));
  network::PacketHeader header;

  acknowledgements.close();
  acknowledgements.acknowledgeIn(header);

  EXPECT_EQ(header, network::PacketHeader{});
}

/**
 * Given a session that accepted a datagram holding guaranteed messages
 * When the session closes, and ACKNOWLEDGEMENT_DELAY passes
 * Then no acknowledgement is due
 */
TEST(SessionAcknowledgements, ForgetTheWaitWhenTheSessionCloses) {
  SessionAcknowledgements acknowledgements;
  acknowledgements.open(ACCEPTANCE_SEQUENCE);
  ASSERT_TRUE(acknowledgements.accept(guaranteedSessionHeader(FIRST_SEQUENCE)));

  acknowledgements.close();
  acknowledgements.advance(PROTOCOL_ACKNOWLEDGEMENT_DELAY);

  EXPECT_FALSE(acknowledgements.isAcknowledgementDue());
}

}  // namespace rtype::client
