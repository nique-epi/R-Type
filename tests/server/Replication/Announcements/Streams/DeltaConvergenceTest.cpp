#include <gtest/gtest.h>
#include "DeltaConvergenceFixture.hpp"
#include "DeltaConvergenceTestConstants.hpp"

namespace rtype::server::testing {

/**
 * Given a client that loses one datagram in three and thirty in a row, and
 * acknowledges what it accepts
 * When ten seconds of a game of 30 enemies, half of them moving, are played
 * Then every state the client rebuilt lists the enemies where the world had
 * them at that tick
 */
TEST_F(DeltaConvergenceFixture, RebuildsEveryStateAsTheWorldHadIt) {
  const ConvergenceReport report = playOverALossyNetwork();

  EXPECT_GT(report.rebuilt, 0U);
  EXPECT_EQ(report.mismatching, 0U);
}

/**
 * Given the same lossy network
 * When ten seconds are played
 * Then the client rebuilds every state whose datagram arrived: a state the
 * client accepted and acknowledged is one it holds, so the server never writes
 * a delta against a state the client lacks
 */
TEST_F(DeltaConvergenceFixture, RebuildsEveryStateThatArrived) {
  const ConvergenceReport report = playOverALossyNetwork();

  EXPECT_EQ(report.rebuilt, report.delivered);
}

/**
 * Given the same lossy network
 * When ten seconds are played
 * Then most states are deltas, and the states after the outage that emptied
 * the acknowledgements are in full again
 */
TEST_F(DeltaConvergenceFixture, FallsBackToFullStatesAfterAnOutage) {
  const ConvergenceReport report = playOverALossyNetwork();

  EXPECT_GE(report.rebuiltFromDeltas, LEAST_STATES_FROM_DELTAS);
  EXPECT_GE(report.rebuiltInFull, 2U);
}

}  // namespace rtype::server::testing
