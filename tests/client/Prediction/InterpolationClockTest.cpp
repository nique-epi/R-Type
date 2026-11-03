#include <gtest/gtest.h>
#include <cstdint>
#include "InterpolationClock.hpp"
#include "InterpolationTestConstants.hpp"

using rtype::client::InterpolationClock;
using rtype::client::testing::A_TICK_BETWEEN_THE_STATES;
using rtype::client::testing::A_TICK_FAR_AFTER_THE_STATES;
using rtype::client::testing::FIFTH_STATE_TICK;
using rtype::client::testing::FIRST_STATE_TICK;
using rtype::client::testing::FOURTH_STATE_TICK;
using rtype::client::testing::NO_SHOWN_TICK;
using rtype::client::testing::ONE_STATE_SHOWN_TICK;
using rtype::client::testing::ONE_TICK;
using rtype::client::testing::ONE_TICK_LATER_SHOWN_TICK;
using rtype::client::testing::SECOND_STATE_TICK;
using rtype::client::testing::SHOWN_TICK_TOLERANCE;
using rtype::client::testing::SIX_TICKS_AFTER_THE_FIRST_STATE;
using rtype::client::testing::TEN_SECONDS;
using rtype::client::testing::THIRD_STATE_TICK;
using rtype::client::testing::TICKS_BETWEEN_STATES;
using rtype::client::testing::TWELVE_TICKS_BEHIND_THE_FAR_STATE;
using rtype::client::testing::TWELVE_TICKS_BEHIND_THE_FIFTH_STATE;
using rtype::client::testing::TWELVE_TICKS_BEHIND_THE_FOURTH_STATE;

/**
 * Given a clock that received no state
 * When it is asked for the tick to show, and advanced
 * Then it shows nothing
 */
TEST(InterpolationClock, ShowsNothingBeforeTheFirstState) {
  InterpolationClock clock;

  clock.advance(ONE_TICK);

  EXPECT_FALSE(clock.renderTick().has_value());
}

/**
 * Given a clock that received a single state, of tick 60
 * When it is asked for the tick to show
 * Then it is 6 ticks behind, two assumed intervals of 3 ticks
 */
TEST(InterpolationClock, StartsTwoAssumedIntervalsBehindTheFirstState) {
  InterpolationClock clock;

  clock.observe(FIRST_STATE_TICK);

  ASSERT_TRUE(clock.renderTick().has_value());
  EXPECT_NEAR(clock.renderTick().value_or(NO_SHOWN_TICK), ONE_STATE_SHOWN_TICK,
              SHOWN_TICK_TOLERANCE);
}

/**
 * Given a clock that received a single state
 * When it advances by the real time of one tick
 * Then the tick it shows is one further
 */
TEST(InterpolationClock, AdvancesWithTheRealTimeOfTheFrames) {
  InterpolationClock clock;
  clock.observe(FIRST_STATE_TICK);

  clock.advance(ONE_TICK);

  ASSERT_TRUE(clock.renderTick().has_value());
  EXPECT_NEAR(clock.renderTick().value_or(NO_SHOWN_TICK),
              ONE_TICK_LATER_SHOWN_TICK, SHOWN_TICK_TOLERANCE);
}

/**
 * Given states 6 ticks apart, each followed by 6 frames of one tick
 * When the fourth state arrives
 * Then the tick shown is 12 ticks behind it, two measured intervals
 */
TEST(InterpolationClock, FollowsTheRateOfTheStatesWithoutASetting) {
  InterpolationClock clock;
  for (const std::uint32_t tick :
       {FIRST_STATE_TICK, SECOND_STATE_TICK, THIRD_STATE_TICK}) {
    clock.observe(tick);
    for (std::uint32_t frame = 0; frame < TICKS_BETWEEN_STATES; ++frame) {
      clock.advance(ONE_TICK);
    }
  }

  clock.observe(FOURTH_STATE_TICK);

  ASSERT_TRUE(clock.renderTick().has_value());
  EXPECT_NEAR(clock.renderTick().value_or(NO_SHOWN_TICK),
              TWELVE_TICKS_BEHIND_THE_FOURTH_STATE, SHOWN_TICK_TOLERANCE);
}

/**
 * Given states 6 ticks apart, the one of tick 72 lost, and the frames between
 * When the state of tick 78 arrives, 12 ticks after the one of tick 66
 * Then the tick shown is still 12 ticks behind the newest: the gap of 12 does
 * not make the delay larger
 */
TEST(InterpolationClock, ALostStateDoesNotWidenTheDelay) {
  InterpolationClock clock;
  for (const std::uint32_t tick : {FIRST_STATE_TICK, SECOND_STATE_TICK}) {
    clock.observe(tick);
    for (std::uint32_t frame = 0; frame < TICKS_BETWEEN_STATES; ++frame) {
      clock.advance(ONE_TICK);
    }
  }
  for (std::uint32_t frame = 0; frame < TICKS_BETWEEN_STATES; ++frame) {
    clock.advance(ONE_TICK);
  }

  clock.observe(FOURTH_STATE_TICK);

  ASSERT_TRUE(clock.renderTick().has_value());
  EXPECT_NEAR(clock.renderTick().value_or(NO_SHOWN_TICK),
              TWELVE_TICKS_BEHIND_THE_FOURTH_STATE, SHOWN_TICK_TOLERANCE);
}

/**
 * Given a clock that received the states of ticks 60 and 66
 * When a late copy of tick 63 and a copy of tick 66 arrive
 * Then the tick shown does not change
 */
TEST(InterpolationClock, IgnoresAStateThatIsNotNewerThanTheNewest) {
  InterpolationClock clock;
  clock.observe(FIRST_STATE_TICK);
  clock.observe(SECOND_STATE_TICK);
  const double before = clock.renderTick().value_or(NO_SHOWN_TICK);

  clock.observe(A_TICK_BETWEEN_THE_STATES);
  clock.observe(SECOND_STATE_TICK);

  EXPECT_NEAR(clock.renderTick().value_or(NO_SHOWN_TICK), before,
              SHOWN_TICK_TOLERANCE);
}

/**
 * Given a clock that received a single state
 * When ten seconds pass with no other state
 * Then the tick shown stops 6 ticks after the state, the extrapolation limit
 */
TEST(InterpolationClock, DoesNotRunAheadWhenNoStateArrives) {
  InterpolationClock clock;
  clock.observe(FIRST_STATE_TICK);

  clock.advance(TEN_SECONDS);

  EXPECT_NEAR(clock.renderTick().value_or(NO_SHOWN_TICK),
              SIX_TICKS_AFTER_THE_FIRST_STATE, SHOWN_TICK_TOLERANCE);
}

/**
 * Given a clock that measured an interval of 6 ticks and then went silent
 * When a state of tick 600 arrives
 * Then the tick shown is taken at once to 12 ticks behind it
 */
TEST(InterpolationClock, TakesBackAFarOffErrorAtOnce) {
  InterpolationClock clock;
  clock.observe(FIRST_STATE_TICK);
  clock.observe(SECOND_STATE_TICK);
  clock.advance(TEN_SECONDS);

  clock.observe(A_TICK_FAR_AFTER_THE_STATES);

  EXPECT_NEAR(clock.renderTick().value_or(NO_SHOWN_TICK),
              TWELVE_TICKS_BEHIND_THE_FAR_STATE, SHOWN_TICK_TOLERANCE);
}

/**
 * Given states 6 ticks apart, each followed by 6 frames of one tick
 * When the fifth state arrives
 * Then the tick shown is 12 ticks behind it, as at the fourth: the delay holds
 */
TEST(InterpolationClock, KeepsTheDelayWhileTheStatesArriveOnTime) {
  InterpolationClock clock;
  for (const std::uint32_t tick : {FIRST_STATE_TICK, SECOND_STATE_TICK,
                                   THIRD_STATE_TICK, FOURTH_STATE_TICK}) {
    clock.observe(tick);
    for (std::uint32_t frame = 0; frame < TICKS_BETWEEN_STATES; ++frame) {
      clock.advance(ONE_TICK);
    }
  }

  clock.observe(FIFTH_STATE_TICK);

  EXPECT_NEAR(clock.renderTick().value_or(NO_SHOWN_TICK),
              TWELVE_TICKS_BEHIND_THE_FIFTH_STATE, SHOWN_TICK_TOLERANCE);
}
