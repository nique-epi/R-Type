#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include "ServerTestConstants.hpp"
#include "TickDurationSummary.hpp"
#include "TickDurations.hpp"
#include "TickDurationsTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

using testing::FULL_BUFFER_AVERAGE;
using testing::FULL_BUFFER_MEDIAN;
using testing::FULL_BUFFER_NINETY_FIFTH_PERCENTILE;
using testing::FULL_BUFFER_WORST;
using testing::LONG_TICK;
using testing::SHORT_AND_LONG_AVERAGE;
using testing::SHORT_TICK;
using testing::SLOWER_THAN_THE_FULL_BUFFER;
using testing::SLOWEST_TICK_MILLISECONDS;

/**
 * Given no tick kept
 * When the summary is taken
 * Then every figure is zero
 */
TEST(TickDurations, TheSummaryOfNoTickIsAllZero) {
  TickDurations durations;

  const TickDurationSummary summary = durations.takeSummary();

  EXPECT_EQ(summary.count, 0U);
  EXPECT_EQ(summary.average, engine::Duration::zero());
  EXPECT_EQ(summary.median, engine::Duration::zero());
  EXPECT_EQ(summary.ninetyFifthPercentile, engine::Duration::zero());
  EXPECT_EQ(summary.worst, engine::Duration::zero());
}

/**
 * Given one tick kept
 * When the summary is taken
 * Then every figure is the duration of that tick
 */
TEST(TickDurations, TheSummaryOfOneTickIsThatTick) {
  TickDurations durations;
  durations.record(SHORT_TICK);

  const TickDurationSummary summary = durations.takeSummary();

  EXPECT_EQ(summary.count, 1U);
  EXPECT_EQ(summary.average, SHORT_TICK);
  EXPECT_EQ(summary.median, SHORT_TICK);
  EXPECT_EQ(summary.ninetyFifthPercentile, SHORT_TICK);
  EXPECT_EQ(summary.worst, SHORT_TICK);
}

/**
 * Given two ticks kept
 * When the summary is taken
 * Then the median is the shorter one, rank 1, and the 95th percentile the
 * longer one, rank 2
 */
TEST(TickDurations, TheSummaryOfTwoTicksFollowsTheNearestRank) {
  TickDurations durations;
  durations.record(LONG_TICK);
  durations.record(SHORT_TICK);

  const TickDurationSummary summary = durations.takeSummary();

  EXPECT_EQ(summary.average, SHORT_AND_LONG_AVERAGE);
  EXPECT_EQ(summary.median, SHORT_TICK);
  EXPECT_EQ(summary.ninetyFifthPercentile, LONG_TICK);
  EXPECT_EQ(summary.worst, LONG_TICK);
}

/**
 * Given ticks of 600, 599, ... 1 ms, the slowest first
 * When the summary is taken
 * Then the figures are those of the ticks in order of duration
 */
TEST(TickDurations, TheSummaryOfAFullBufferFollowsTheNearestRank) {
  TickDurations durations;
  for (int milliseconds = SLOWEST_TICK_MILLISECONDS; milliseconds >= 1;
       --milliseconds) {
    durations.record(std::chrono::milliseconds{milliseconds});
  }

  const TickDurationSummary summary = durations.takeSummary();

  EXPECT_EQ(summary.count, TICKS_IN_TEN_SECONDS);
  EXPECT_EQ(summary.average, FULL_BUFFER_AVERAGE);
  EXPECT_EQ(summary.median, FULL_BUFFER_MEDIAN);
  EXPECT_EQ(summary.ninetyFifthPercentile, FULL_BUFFER_NINETY_FIFTH_PERCENTILE);
  EXPECT_EQ(summary.worst, FULL_BUFFER_WORST);
}

/**
 * Given the ticks of ten seconds but one
 * When the buffer is asked whether it is full
 * Then it is not
 */
TEST(TickDurations, IsNotFullOneTickBeforeTenSeconds) {
  TickDurations durations;
  for (std::size_t tick = 1; tick < TICKS_IN_TEN_SECONDS; ++tick) {
    durations.record(SHORT_TICK);
  }

  EXPECT_FALSE(durations.isFull());
}

/**
 * Given the ticks of ten seconds
 * When the buffer is asked whether it is full
 * Then it is
 */
TEST(TickDurations, IsFullAfterTenSeconds) {
  TickDurations durations;
  for (std::size_t tick = 0; tick < TICKS_IN_TEN_SECONDS; ++tick) {
    durations.record(SHORT_TICK);
  }

  EXPECT_TRUE(durations.isFull());
}

/**
 * Given a full buffer
 * When one more tick, slower than the others, is recorded
 * Then it is not kept
 */
TEST(TickDurations, KeepsNoTickOnceFull) {
  TickDurations durations;
  for (int milliseconds = 1; milliseconds <= SLOWEST_TICK_MILLISECONDS;
       ++milliseconds) {
    durations.record(std::chrono::milliseconds{milliseconds});
  }

  durations.record(SLOWER_THAN_THE_FULL_BUFFER);

  const TickDurationSummary summary = durations.takeSummary();
  EXPECT_EQ(summary.count, TICKS_IN_TEN_SECONDS);
  EXPECT_EQ(summary.worst, FULL_BUFFER_WORST);
}

/**
 * Given a full buffer
 * When its summary is taken
 * Then it is empty again, with room for the next ticks
 */
TEST(TickDurations, TakingTheSummaryForgetsTheTicks) {
  TickDurations durations;
  for (std::size_t tick = 0; tick < TICKS_IN_TEN_SECONDS; ++tick) {
    durations.record(SHORT_TICK);
  }

  [[maybe_unused]] const TickDurationSummary summary = durations.takeSummary();

  EXPECT_FALSE(durations.isFull());
  EXPECT_EQ(durations.takeSummary().count, 0U);
}

}  // namespace rtype::server
