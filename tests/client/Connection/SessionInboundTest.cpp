#include <gtest/gtest.h>
#include "Reception.hpp"
#include "ReceptionKind.hpp"
#include "ServerSessionTestConstants.hpp"
#include "SessionInboundFixture.hpp"

namespace rtype::client {

using testing::ACCEPTANCE_SEQUENCE;
using testing::FIRST_SEQUENCE;
using testing::OTHER_SESSION_TOKEN;
using testing::SessionInboundFixture;

/**
 * Given a session opened with a session token
 * When a datagram of the server then carries another token
 * Then it is ignored and counted
 */
TEST_F(SessionInboundFixture, CountsADatagramWithAnotherTokenOnceOpen) {
  openSession();

  const Reception reception = receiveWithToken(OTHER_SESSION_TOKEN);

  EXPECT_EQ(reception.kind, ReceptionKind::Ignored);
  EXPECT_EQ(wrongTokenCount(), 1);
}

/**
 * Given a session opened with a session token
 * When a datagram of the server then carries the same token, as old as the
 * acceptance
 * Then it is ignored as a copy, and nothing is counted
 */
TEST_F(SessionInboundFixture, CountsNothingForTheTokenOfTheAcceptance) {
  openSession();

  const Reception reception = receiveWithSequence(ACCEPTANCE_SEQUENCE);

  EXPECT_EQ(reception.kind, ReceptionKind::Ignored);
  EXPECT_EQ(wrongTokenCount(), 0);
}

/**
 * Given a session opened with a session token
 * When a datagram of the server carries the same token and a newer sequence
 * Then it is read
 */
TEST_F(SessionInboundFixture,
       ReadsADatagramOfTheSessionNewerThanTheAcceptance) {
  openSession();

  const Reception reception = receiveWithSequence(FIRST_SEQUENCE);

  EXPECT_EQ(reception.kind, ReceptionKind::Read);
}

}  // namespace rtype::client
