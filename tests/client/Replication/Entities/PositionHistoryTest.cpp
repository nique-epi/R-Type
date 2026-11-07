#include <gtest/gtest.h>
#include <cstdint>
#include "PositionHistory.hpp"
#include "PositionHistoryTestConstants.hpp"
#include "PositionSample.hpp"

using rtype::client::PositionHistory;
using rtype::client::PositionSample;
using rtype::client::testing::AT_THE_EXTRAPOLATION_LIMIT;
using rtype::client::testing::BEFORE_EVERY_SAMPLE_TICK;
using rtype::client::testing::BEFORE_THE_FIRST_SAMPLE_TICK;
using rtype::client::testing::FAR_AHEAD_TICK;
using rtype::client::testing::FIFTH_TICK;
using rtype::client::testing::FIRST_SAMPLE;
using rtype::client::testing::HALFWAY_POSITION;
using rtype::client::testing::HALFWAY_TICK;
using rtype::client::testing::LATE_POSITION;
using rtype::client::testing::LATE_TICK;
using rtype::client::testing::NO_POSITION;
using rtype::client::testing::OLDEST_KEPT_TICK;
using rtype::client::testing::ONE_AND_A_HALF_TICKS_AHEAD_POSITION;
using rtype::client::testing::ONE_AND_A_HALF_TICKS_AHEAD_TICK;
using rtype::client::testing::SECOND_SAMPLE;
using rtype::client::testing::SECOND_SAMPLE_X;
using rtype::client::testing::UNITS_PER_SAMPLE_TICK;

/**
 * Given a history that holds no sample
 * When it is asked where the entity is
 * Then it answers nothing
 */
TEST(PositionHistory, ShowsNothingWithoutASample) {
  const PositionHistory history;

  EXPECT_FALSE(history.at(HALFWAY_TICK).has_value());
}

/**
 * Given two samples, at ticks 6 and 9
 * When the entity is asked for at tick 7.5, halfway between them
 * Then it is halfway between the two positions
 */
TEST(PositionHistory, InterpolatesLinearlyBetweenTwoSamples) {
  PositionHistory history;
  history.record(FIRST_SAMPLE);
  history.record(SECOND_SAMPLE);

  const auto position = history.at(HALFWAY_TICK);

  ASSERT_TRUE(position.has_value());
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).x, HALFWAY_POSITION.x);
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).y, HALFWAY_POSITION.y);
}

/**
 * Given two samples, at ticks 6 and 9
 * When the entity is asked for at tick 2, before the oldest
 * Then it is where the oldest sample put it
 */
TEST(PositionHistory, HoldsTheOldestSampleBeforeIt) {
  PositionHistory history;
  history.record(FIRST_SAMPLE);
  history.record(SECOND_SAMPLE);

  const auto position = history.at(BEFORE_THE_FIRST_SAMPLE_TICK);

  ASSERT_TRUE(position.has_value());
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).x, FIRST_SAMPLE.position.x);
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).y, FIRST_SAMPLE.position.y);
}

/**
 * Given two samples, at ticks 6 and 9
 * When the entity is asked for at tick 10.5, after the newest
 * Then it is moved on at the speed between the two samples
 */
TEST(PositionHistory, MovesOnAtTheLastSpeedAfterTheNewestSample) {
  PositionHistory history;
  history.record(FIRST_SAMPLE);
  history.record(SECOND_SAMPLE);

  const auto position = history.at(ONE_AND_A_HALF_TICKS_AHEAD_TICK);

  ASSERT_TRUE(position.has_value());
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).x,
                  ONE_AND_A_HALF_TICKS_AHEAD_POSITION.x);
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).y,
                  ONE_AND_A_HALF_TICKS_AHEAD_POSITION.y);
}

/**
 * Given two samples, at ticks 6 and 9
 * When the entity is asked for at tick 109, 100 ticks after the newest
 * Then it is where it would be 6 ticks after the newest, no further
 */
TEST(PositionHistory, StopsMovingOnAtTheExtrapolationLimit) {
  PositionHistory history;
  history.record(FIRST_SAMPLE);
  history.record(SECOND_SAMPLE);

  const auto position = history.at(FAR_AHEAD_TICK);

  ASSERT_TRUE(position.has_value());
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).x,
                  AT_THE_EXTRAPOLATION_LIMIT.x);
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).y,
                  AT_THE_EXTRAPOLATION_LIMIT.y);
}

/**
 * Given a single sample
 * When the entity is asked for at a tick after it
 * Then it stays where the sample put it, since no speed is known
 */
TEST(PositionHistory, StaysWhereASingleSampleIsAfterIt) {
  PositionHistory history;
  history.record(FIRST_SAMPLE);

  const auto position = history.at(FAR_AHEAD_TICK);

  ASSERT_TRUE(position.has_value());
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).x, FIRST_SAMPLE.position.x);
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).y, FIRST_SAMPLE.position.y);
}

/**
 * Given samples at ticks 6 and 9
 * When a sample of tick 7 arrives late, then another of tick 9 again
 * Then the entity, asked for at tick 10.5, after the newest sample, is still
 * moved on at the speed of the samples of ticks 6 and 9
 */
TEST(PositionHistory, IgnoresASampleThatIsNotNewerThanTheNewest) {
  PositionHistory history;
  history.record(FIRST_SAMPLE);
  history.record(SECOND_SAMPLE);

  history.record(PositionSample{.tick = LATE_TICK, .position = LATE_POSITION});
  history.record(
      PositionSample{.tick = SECOND_SAMPLE.tick, .position = LATE_POSITION});

  const auto position = history.at(ONE_AND_A_HALF_TICKS_AHEAD_TICK);
  ASSERT_TRUE(position.has_value());
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).x,
                  ONE_AND_A_HALF_TICKS_AHEAD_POSITION.x);
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).y,
                  ONE_AND_A_HALF_TICKS_AHEAD_POSITION.y);
}

/**
 * Given five samples recorded, at ticks 1 to 5 and 10 units further each
 * When the entity is asked for at tick 0, before every sample kept
 * Then it is where the second sample put it: the first was forgotten
 */
TEST(PositionHistory, OnlyKeepsTheNewestSamples) {
  PositionHistory history;
  for (std::uint32_t tick = 1; tick <= FIFTH_TICK; ++tick) {
    history.record(PositionSample{
        .tick = tick,
        .position = {.x = UNITS_PER_SAMPLE_TICK * static_cast<float>(tick),
                     .y = 0.0F}});
  }

  const auto position = history.at(BEFORE_EVERY_SAMPLE_TICK);

  ASSERT_TRUE(position.has_value());
  EXPECT_FLOAT_EQ(position.value_or(NO_POSITION).x, SECOND_SAMPLE_X);
}

/**
 * Given a history that holds no sample
 * When it is asked for the tick of its oldest sample
 * Then it answers nothing
 */
TEST(PositionHistory, HasNoOldestTickWithoutASample) {
  const PositionHistory history;

  EXPECT_FALSE(history.oldestTick().has_value());
}

/**
 * Given five samples recorded, at ticks 1 to 5
 * When the history is asked for the tick of its oldest sample
 * Then it answers tick 2: the sample of tick 1 was forgotten
 */
TEST(PositionHistory, GivesTheTickOfTheOldestSampleKept) {
  PositionHistory history;
  for (std::uint32_t tick = 1; tick <= FIFTH_TICK; ++tick) {
    history.record(PositionSample{.tick = tick, .position = {}});
  }

  const auto oldest = history.oldestTick();

  ASSERT_TRUE(oldest.has_value());
  EXPECT_EQ(oldest.value_or(0), OLDEST_KEPT_TICK);
}
