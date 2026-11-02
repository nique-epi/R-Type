#include <gtest/gtest.h>
#include "DurationConversion.hpp"
#include "DurationConversionTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::engine::averageOf;
using rtype::engine::Duration;
using rtype::engine::inMilliseconds;

/**
 * Given a duration of one and a half milliseconds
 * When it is converted
 * Then the fraction of a millisecond is kept
 */
TEST(DurationConversion, KeepsTheFractionOfAMillisecond) {
  EXPECT_DOUBLE_EQ(inMilliseconds(ONE_AND_A_HALF_MILLISECONDS), ONE_AND_A_HALF);
}

/**
 * Given no time at all
 * When it is converted
 * Then it is zero milliseconds
 */
TEST(DurationConversion, NoTimeIsZeroMilliseconds) {
  EXPECT_DOUBLE_EQ(inMilliseconds(Duration::zero()), 0.0);
}

/**
 * Given a total of six milliseconds
 * When it is shared between three parts
 * Then each part is two milliseconds
 */
TEST(DurationConversion, SharesATotalEquallyBetweenItsParts) {
  EXPECT_EQ(averageOf(TOTAL_OF_THREE_PARTS, THREE_PARTS), SHARE_OF_ONE_PART);
}

/**
 * Given a total and no part to share it between
 * When the average is asked for
 * Then it is zero: computed at compile time, where a division by zero is an
 * error on every processor
 */
TEST(DurationConversion, TheAverageOverNoPartIsZero) {
  constexpr Duration AVERAGE_OVER_NO_PART = averageOf(TOTAL_OF_THREE_PARTS, 0);

  EXPECT_EQ(AVERAGE_OVER_NO_PART, Duration::zero());
}
