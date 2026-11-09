#include <gtest/gtest.h>
#include "ArrivalTally.hpp"
#include "ArrivalTallyTestConstants.hpp"
#include "SessionArrivals.hpp"

namespace rtype::client {

using testing::FARTHEST_NEWER_DISTANCE;
using testing::MOST_SKIPPED;
using testing::NEXT_DATAGRAM_DISTANCE;
using testing::THREE_SEQUENCES_AHEAD;
using testing::TWO_SKIPPED;

/**
 * Given a tally that counted nothing
 * When it is read
 * Then nothing was accepted and nothing missed
 */
TEST(ArrivalTally, StartsEmpty) {
  const ArrivalTally tally;

  EXPECT_EQ(tally.arrivals(), SessionArrivals{});
}

/**
 * Given a tally
 * When a datagram right after the newest is counted
 * Then one datagram was accepted and none missed
 */
TEST(ArrivalTally, TheNextDatagramMissesNothing) {
  ArrivalTally tally;

  tally.countAccepted(NEXT_DATAGRAM_DISTANCE);

  EXPECT_EQ(tally.arrivals(), (SessionArrivals{.accepted = 1, .missed = 0}));
}

/**
 * Given a tally
 * When a datagram three sequences after the newest is counted
 * Then the two sequences between them are missed
 */
TEST(ArrivalTally, ADatagramAheadMissesTheSequencesBetween) {
  ArrivalTally tally;

  tally.countAccepted(THREE_SEQUENCES_AHEAD);

  EXPECT_EQ(tally.arrivals(),
            (SessionArrivals{.accepted = 1, .missed = TWO_SKIPPED}));
}

/**
 * Given a tally
 * When a datagram as far ahead as a newer one can be is counted
 * Then every sequence between them is missed, 32766
 */
TEST(ArrivalTally, TheFarthestNewerDatagramMissesEverySequenceBetween) {
  ArrivalTally tally;

  tally.countAccepted(FARTHEST_NEWER_DISTANCE);

  EXPECT_EQ(tally.arrivals(),
            (SessionArrivals{.accepted = 1, .missed = MOST_SKIPPED}));
}

}  // namespace rtype::client
