#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "EngineException.hpp"
#include "FixedTimestep.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"
#include "TimerHandle.hpp"
#include "TimerScheduler.hpp"

using rtype::engine::Duration;
using rtype::engine::FixedTimestep;
using rtype::engine::InvalidTimerDelayException;
using rtype::engine::InvalidTimerIntervalException;
using rtype::engine::NegativeElapsedTimeException;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::engine::TimerHandle;
using rtype::engine::TimerScheduler;

namespace {

constexpr Duration SHORT_DELAY(10'000'000);
constexpr Duration LONG_DELAY(20'000'000);
constexpr Duration ONE_NANOSECOND(1);
constexpr Duration HALF_SECOND(500'000'000);
constexpr int FRAMES_BEFORE_FIRST_FIRE = 24;
constexpr int FRAMES_AFTER_SECOND_FIRE = 66;
constexpr int ONE_CALL = 1;
constexpr int TWO_CALLS = 2;
constexpr int THREE_CALLS = 3;

}  // namespace

/**
 * Given a one-shot timer
 * When the simulation advances by less than its delay
 * Then the callback has not run
 */
TEST(TimerScheduler, OneShotDoesNotFireBeforeItsDelay) {
  TimerScheduler scheduler;
  int calls = 0;
  scheduler.scheduleOnce(SHORT_DELAY, [&calls] { ++calls; });

  scheduler.advance(SHORT_DELAY - ONE_NANOSECOND);

  EXPECT_EQ(calls, 0);
}

/**
 * Given a one-shot timer
 * When the simulation advances by exactly its delay
 * Then the callback runs once
 */
TEST(TimerScheduler, OneShotFiresWhenItsDelayHasElapsed) {
  TimerScheduler scheduler;
  int calls = 0;
  scheduler.scheduleOnce(SHORT_DELAY, [&calls] { ++calls; });

  scheduler.advance(SHORT_DELAY);

  EXPECT_EQ(calls, ONE_CALL);
}

/**
 * Given a one-shot timer that has fired
 * When the simulation keeps advancing
 * Then the callback does not run again and the timer is gone
 */
TEST(TimerScheduler, OneShotFiresOnlyOnce) {
  TimerScheduler scheduler;
  int calls = 0;
  scheduler.scheduleOnce(SHORT_DELAY, [&calls] { ++calls; });

  scheduler.advance(SHORT_DELAY);
  scheduler.advance(LONG_DELAY);

  EXPECT_EQ(calls, ONE_CALL);
  EXPECT_EQ(scheduler.size(), 0U);
}

/**
 * Given a one-shot timer with a zero delay
 * When the simulation advances by zero
 * Then the callback runs once
 */
TEST(TimerScheduler, ZeroDelayFiresOnNextAdvance) {
  TimerScheduler scheduler;
  int calls = 0;
  scheduler.scheduleOnce(Duration::zero(), [&calls] { ++calls; });

  scheduler.advance(Duration::zero());

  EXPECT_EQ(calls, ONE_CALL);
}

/**
 * Given a repeating timer
 * When the simulation advances by one interval, three times
 * Then the callback runs three times and the timer stays pending
 */
TEST(TimerScheduler, RepeatingFiresOncePerInterval) {
  TimerScheduler scheduler;
  int calls = 0;
  scheduler.scheduleRepeating(SHORT_DELAY, [&calls] { ++calls; });

  scheduler.advance(SHORT_DELAY);
  scheduler.advance(SHORT_DELAY);
  scheduler.advance(SHORT_DELAY);

  EXPECT_EQ(calls, THREE_CALLS);
  EXPECT_EQ(scheduler.size(), 1U);
}

/**
 * Given a repeating timer
 * When the simulation advances by three and a half intervals at once
 * Then the callback runs three times
 */
TEST(TimerScheduler, RepeatingCatchesUpWithinOneAdvance) {
  TimerScheduler scheduler;
  int calls = 0;
  scheduler.scheduleRepeating(SHORT_DELAY, [&calls] { ++calls; });

  scheduler.advance(SHORT_DELAY * THREE_CALLS + SHORT_DELAY / 2);

  EXPECT_EQ(calls, THREE_CALLS);
}

/**
 * Given a repeating timer whose half interval was already consumed
 * When the simulation advances by another half interval
 * Then the callback runs once, so the remainder is not lost
 */
TEST(TimerScheduler, RepeatingKeepsTheRemainderBetweenAdvances) {
  TimerScheduler scheduler;
  int calls = 0;
  scheduler.scheduleRepeating(SHORT_DELAY, [&calls] { ++calls; });

  scheduler.advance(SHORT_DELAY / 2);
  scheduler.advance(SHORT_DELAY / 2);

  EXPECT_EQ(calls, ONE_CALL);
}

/**
 * Given a pending one-shot timer
 * When it is cancelled before its delay
 * Then the cancel succeeds, the callback never runs and the timer is gone
 */
TEST(TimerScheduler, CancelledOneShotNeverFires) {
  TimerScheduler scheduler;
  int calls = 0;
  const TimerHandle handle =
      scheduler.scheduleOnce(SHORT_DELAY, [&calls] { ++calls; });

  const bool cancelled = scheduler.cancel(handle);
  scheduler.advance(LONG_DELAY);

  EXPECT_TRUE(cancelled);
  EXPECT_EQ(calls, 0);
  EXPECT_EQ(scheduler.size(), 0U);
}

/**
 * Given a repeating timer that has fired once
 * When it is cancelled
 * Then it does not fire again
 */
TEST(TimerScheduler, CancelledRepeatingStopsFiring) {
  TimerScheduler scheduler;
  int calls = 0;
  const TimerHandle handle =
      scheduler.scheduleRepeating(SHORT_DELAY, [&calls] { ++calls; });
  scheduler.advance(SHORT_DELAY);

  scheduler.cancel(handle);
  scheduler.advance(LONG_DELAY);

  EXPECT_EQ(calls, ONE_CALL);
}

/**
 * Given a one-shot timer that already fired
 * When it is cancelled
 * Then the cancel reports that nothing was pending
 */
TEST(TimerScheduler, CancellingAFiredTimerReportsFalse) {
  TimerScheduler scheduler;
  const TimerHandle handle = scheduler.scheduleOnce(SHORT_DELAY, [] {});
  scheduler.advance(SHORT_DELAY);

  EXPECT_FALSE(scheduler.cancel(handle));
}

/**
 * Given a handle that was never issued by the scheduler
 * When it is cancelled
 * Then the cancel reports that nothing was pending
 */
TEST(TimerScheduler, CancellingAnUnknownHandleReportsFalse) {
  TimerScheduler scheduler;
  const TimerHandle unknown{.identifier = 42};

  EXPECT_FALSE(scheduler.cancel(unknown));
}

/**
 * Given a timer that was cancelled
 * When it is cancelled a second time
 * Then the second cancel reports that nothing was pending
 */
TEST(TimerScheduler, CancellingTwiceReportsFalse) {
  TimerScheduler scheduler;
  const TimerHandle handle = scheduler.scheduleOnce(SHORT_DELAY, [] {});
  scheduler.cancel(handle);

  EXPECT_FALSE(scheduler.cancel(handle));
}

/**
 * Given the handle of a one-shot timer that already fired, and a newer timer
 * When the stale handle is cancelled
 * Then the newer timer is untouched and still fires
 */
TEST(TimerScheduler, StaleHandleNeverCancelsANewerTimer) {
  TimerScheduler scheduler;
  int calls = 0;
  const TimerHandle stale = scheduler.scheduleOnce(SHORT_DELAY, [] {});
  scheduler.advance(SHORT_DELAY);
  scheduler.scheduleOnce(SHORT_DELAY, [&calls] { ++calls; });

  scheduler.cancel(stale);
  scheduler.advance(SHORT_DELAY);

  EXPECT_EQ(calls, ONE_CALL);
}

/**
 * Given two timers scheduled with the later one first
 * When the simulation advances past both
 * Then they fire in order of due time
 */
TEST(TimerScheduler, TimersFireInOrderOfDueTime) {
  TimerScheduler scheduler;
  std::vector<int> order;
  scheduler.scheduleOnce(LONG_DELAY, [&order] { order.push_back(2); });
  scheduler.scheduleOnce(SHORT_DELAY, [&order] { order.push_back(1); });

  scheduler.advance(LONG_DELAY);

  EXPECT_EQ(order, (std::vector<int>{1, 2}));
}

/**
 * Given two timers due at the same instant
 * When the simulation advances to that instant
 * Then they fire in the order they were created
 */
TEST(TimerScheduler, TimersDueTogetherFireInCreationOrder) {
  TimerScheduler scheduler;
  std::vector<int> order;
  scheduler.scheduleOnce(SHORT_DELAY, [&order] { order.push_back(1); });
  scheduler.scheduleOnce(SHORT_DELAY, [&order] { order.push_back(2); });

  scheduler.advance(SHORT_DELAY);

  EXPECT_EQ(order, (std::vector<int>{1, 2}));
}

/**
 * Given a one-shot timer whose callback schedules another one with the same
 * delay
 * When the simulation advances by twice that delay in a single call
 * Then both callbacks run, the second one starting at the first one's due time
 */
TEST(TimerScheduler, TimerScheduledFromACallbackStartsAtItsParentDueTime) {
  TimerScheduler scheduler;
  int calls = 0;
  scheduler.scheduleOnce(SHORT_DELAY, [&scheduler, &calls] {
    ++calls;
    scheduler.scheduleOnce(SHORT_DELAY, [&calls] { ++calls; });
  });

  scheduler.advance(LONG_DELAY);

  EXPECT_EQ(calls, TWO_CALLS);
}

/**
 * Given a repeating timer whose callback cancels it
 * When the simulation advances by several intervals
 * Then the callback ran exactly once and the timer is gone
 */
TEST(TimerScheduler, RepeatingTimerCanCancelItselfFromItsCallback) {
  TimerScheduler scheduler;
  int calls = 0;
  TimerHandle handle{.identifier = 0};
  handle = scheduler.scheduleRepeating(SHORT_DELAY, [&] {
    ++calls;
    scheduler.cancel(handle);
  });

  scheduler.advance(SHORT_DELAY * THREE_CALLS);

  EXPECT_EQ(calls, ONE_CALL);
  EXPECT_EQ(scheduler.size(), 0U);
}

/**
 * Given two timers where the earlier one cancels the later one
 * When the simulation advances past both
 * Then the cancelled timer does not fire
 */
TEST(TimerScheduler, CallbackCanCancelAnotherPendingTimer) {
  TimerScheduler scheduler;
  int cancelledCalls = 0;
  const TimerHandle later = scheduler.scheduleOnce(
      LONG_DELAY, [&cancelledCalls] { ++cancelledCalls; });
  scheduler.scheduleOnce(SHORT_DELAY,
                         [&scheduler, later] { scheduler.cancel(later); });

  scheduler.advance(LONG_DELAY);

  EXPECT_EQ(cancelledCalls, 0);
}

/**
 * Given a scheduler
 * When a negative delay is requested
 * Then it is rejected
 */
TEST(TimerScheduler, NegativeDelayIsRejected) {
  TimerScheduler scheduler;

  EXPECT_THROW(scheduler.scheduleOnce(-ONE_NANOSECOND, [] {}),
               InvalidTimerDelayException);
}

/**
 * Given a scheduler
 * When a repeating timer with a zero interval is requested
 * Then it is rejected
 */
TEST(TimerScheduler, ZeroIntervalIsRejected) {
  TimerScheduler scheduler;

  EXPECT_THROW(scheduler.scheduleRepeating(Duration::zero(), [] {}),
               InvalidTimerIntervalException);
}

/**
 * Given a scheduler
 * When a repeating timer with a negative interval is requested
 * Then it is rejected
 */
TEST(TimerScheduler, NegativeIntervalIsRejected) {
  TimerScheduler scheduler;

  EXPECT_THROW(scheduler.scheduleRepeating(-ONE_NANOSECOND, [] {}),
               InvalidTimerIntervalException);
}

/**
 * Given a scheduler
 * When the simulation is advanced by a negative amount
 * Then it is rejected
 */
TEST(TimerScheduler, NegativeElapsedTimeIsRejected) {
  TimerScheduler scheduler;

  EXPECT_THROW(scheduler.advance(-ONE_NANOSECOND),
               NegativeElapsedTimeException);
}

/**
 * Given a repeating timer fed by the ticks of a simulated clock, one frame
 * per tick
 * When 66 ticks (1.1 seconds) have been rendered
 * Then a half-second timer has fired twice, and not at all after 24 ticks
 */
TEST(TimerScheduler, FollowsSimulatedTimeThroughFixedTimestep) {
  SimulatedClock clock;
  FixedTimestep timestep(clock, SIMULATION_TICK_DURATION);
  TimerScheduler scheduler;
  int calls = 0;
  scheduler.scheduleRepeating(HALF_SECOND, [&calls] { ++calls; });
  const auto renderFrames = [&](int frames) {
    for (int frame = 0; frame < frames; ++frame) {
      clock.advance(SIMULATION_TICK_DURATION);
      for (std::size_t tick = timestep.consumeTicks(); tick > 0; --tick) {
        scheduler.advance(SIMULATION_TICK_DURATION);
      }
    }
  };

  renderFrames(FRAMES_BEFORE_FIRST_FIRE);
  const int callsBeforeFirstFire = calls;
  renderFrames(FRAMES_AFTER_SECOND_FIRE - FRAMES_BEFORE_FIRST_FIRE);

  EXPECT_EQ(callsBeforeFirstFire, 0);
  EXPECT_EQ(calls, TWO_CALLS);
}
