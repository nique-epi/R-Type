#include <gtest/gtest.h>
#include "RetryBackoff.hpp"
#include "RetryBackoffTestConstants.hpp"

using rtype::server::RetryBackoff;
using rtype::server::testing::BACKOFF_MAXIMUM;
using rtype::server::testing::BACKOFF_TIMES;
using rtype::server::testing::FIRST_CEILING;
using rtype::server::testing::FOURTH_CEILING;
using rtype::server::testing::NO_DRAW;
using rtype::server::testing::NO_WAIT;
using rtype::server::testing::QUARTER_DRAW;
using rtype::server::testing::QUARTER_OF_FIRST_CEILING;
using rtype::server::testing::SECOND_CEILING;
using rtype::server::testing::THIRD_CEILING;
using rtype::server::testing::WHOLE_DRAW;

/**
 * Given a backoff that has not waited yet
 * When the first wait draws the whole ceiling
 * Then it waits the first retry delay
 */
TEST(RetryBackoff, FirstWaitIsAtMostTheRetryDelay) {
  RetryBackoff backoff{BACKOFF_TIMES};

  EXPECT_EQ(backoff.nextWait(WHOLE_DRAW), FIRST_CEILING);
}

/**
 * Given a backoff that has not waited yet
 * When three waits in a row draw the whole ceiling
 * Then each one is twice the one before it
 */
TEST(RetryBackoff, EachWaitDoublesTheCeiling) {
  RetryBackoff backoff{BACKOFF_TIMES};
  static_cast<void>(backoff.nextWait(WHOLE_DRAW));

  EXPECT_EQ(backoff.nextWait(WHOLE_DRAW), SECOND_CEILING);
  EXPECT_EQ(backoff.nextWait(WHOLE_DRAW), THIRD_CEILING);
}

/**
 * Given a backoff whose ceiling reached 800 ms, under a maximum of 1000 ms
 * When the next waits draw the whole ceiling
 * Then they wait the maximum, never more
 */
TEST(RetryBackoff, TheCeilingStopsAtTheMaximum) {
  RetryBackoff backoff{BACKOFF_TIMES};
  static_cast<void>(backoff.nextWait(WHOLE_DRAW));
  static_cast<void>(backoff.nextWait(WHOLE_DRAW));
  static_cast<void>(backoff.nextWait(WHOLE_DRAW));
  ASSERT_EQ(backoff.nextWait(WHOLE_DRAW), FOURTH_CEILING);

  EXPECT_EQ(backoff.nextWait(WHOLE_DRAW), BACKOFF_MAXIMUM);
  EXPECT_EQ(backoff.nextWait(WHOLE_DRAW), BACKOFF_MAXIMUM);
}

/**
 * Given a backoff that has not waited yet
 * When the first wait draws a quarter of the ceiling
 * Then it waits a quarter of the first retry delay
 */
TEST(RetryBackoff, AWaitIsTheDrawnShareOfTheCeiling) {
  RetryBackoff backoff{BACKOFF_TIMES};

  EXPECT_EQ(backoff.nextWait(QUARTER_DRAW), QUARTER_OF_FIRST_CEILING);
}

/**
 * Given a backoff that has not waited yet
 * When the first wait draws nothing
 * Then the next registration goes at once
 */
TEST(RetryBackoff, ADrawOfNothingWaitsNothing) {
  RetryBackoff backoff{BACKOFF_TIMES};

  EXPECT_EQ(backoff.nextWait(NO_DRAW), NO_WAIT);
}

/**
 * Given a backoff whose ceiling grew over three waits
 * When it is reset, then waits drawing the whole ceiling
 * Then it waits the first retry delay again
 */
TEST(RetryBackoff, AResetStartsAgainFromTheRetryDelay) {
  RetryBackoff backoff{BACKOFF_TIMES};
  static_cast<void>(backoff.nextWait(WHOLE_DRAW));
  static_cast<void>(backoff.nextWait(WHOLE_DRAW));
  static_cast<void>(backoff.nextWait(WHOLE_DRAW));

  backoff.reset();

  EXPECT_EQ(backoff.nextWait(WHOLE_DRAW), FIRST_CEILING);
}
