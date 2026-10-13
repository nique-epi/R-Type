#include <gtest/gtest.h>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <future>
#include <stop_token>
#include <thread>
#include "ServerTestConstants.hpp"
#include "SimulatedClock.hpp"
#include "SystemClock.hpp"
#include "TickLoop.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Duration;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::server::TickLoop;

namespace {

/**
 * @brief Lets @p tickCount ticks and a half pass at once, then runs the ticks
 * due.
 */
void stallFor(SimulatedClock& clock, TickLoop& loop, int tickCount) {
  clock.advance(SIMULATION_TICK_DURATION * tickCount + HALF_TICK);
  loop.runDueTicks();
}

/**
 * @brief The game time a stall of @p tickCount ticks and a half drops: all of
 * it past the ticks caught up.
 */
Duration droppedBy(int tickCount) {
  return SIMULATION_TICK_DURATION * (tickCount - CAUGHT_UP_TICK_COUNT) +
         HALF_TICK;
}

}  // namespace

/**
 * Given a loop whose clock has not reached the first tick
 * When the due ticks are run
 * Then the tick function is not called
 */
TEST(TickLoop, RunsNothingBeforeTheFirstTickIsDue) {
  SimulatedClock clock;
  int tickCount = 0;
  TickLoop loop{clock, [&tickCount] { ++tickCount; }};
  clock.advance(SIMULATION_TICK_DURATION - Duration(1));

  loop.runDueTicks();

  EXPECT_EQ(tickCount, 0);
}

/**
 * Given a loop whose clock jumped three ticks forward at once
 * When the due ticks are run
 * Then the tick function is called three times
 */
TEST(TickLoop, RunsEveryMissedTickBackToBack) {
  SimulatedClock clock;
  int tickCount = 0;
  TickLoop loop{clock, [&tickCount] { ++tickCount; }};
  clock.advance(SIMULATION_TICK_DURATION * MISSED_TICK_COUNT);

  loop.runDueTicks();

  EXPECT_EQ(tickCount, MISSED_TICK_COUNT);
}

/**
 * Given a loop run exactly when its first tick was due
 * When its lateness is taken
 * Then the tick started zero late
 */
TEST(TickLoop, TickStartedWhenDueIsNotLate) {
  SimulatedClock clock;
  TickLoop loop{clock, [] {}};
  clock.advance(SIMULATION_TICK_DURATION);
  loop.runDueTicks();

  const TickLoop::Lateness lateness = loop.takeLateness();

  EXPECT_EQ(lateness.tickCount, 1U);
  EXPECT_EQ(lateness.maximum, Duration::zero());
}

/**
 * Given a loop first run two ticks and a half after it was created
 * When its lateness is taken
 * Then the first tick started one tick and a half late and the second half a
 * tick late
 */
TEST(TickLoop, MeasuresHowLateEachTickStartedAfterItWasDue) {
  SimulatedClock clock;
  TickLoop loop{clock, [] {}};
  clock.advance(SIMULATION_TICK_DURATION * TICKS_DUE_TOGETHER + HALF_TICK);
  loop.runDueTicks();

  const TickLoop::Lateness lateness = loop.takeLateness();

  EXPECT_EQ(lateness.tickCount, static_cast<std::size_t>(TICKS_DUE_TOGETHER));
  EXPECT_EQ(lateness.maximum, SIMULATION_TICK_DURATION + HALF_TICK);
  EXPECT_EQ(lateness.total, SIMULATION_TICK_DURATION + HALF_TICK + HALF_TICK);
}

/**
 * Given two ticks due at once, the first of which takes half a tick to run
 * When the lateness is taken
 * Then the second tick is late by the time the first one took
 */
TEST(TickLoop, CountsTheTimeEarlierTicksTookInTheLateness) {
  SimulatedClock clock;
  TickLoop loop{clock, [&clock] { clock.advance(HALF_TICK); }};
  clock.advance(SIMULATION_TICK_DURATION * TICKS_DUE_TOGETHER);
  loop.runDueTicks();

  const TickLoop::Lateness lateness = loop.takeLateness();

  EXPECT_EQ(lateness.total, SIMULATION_TICK_DURATION + HALF_TICK);
}

/**
 * Given a loop first run sixty ticks and a half after it was created, so the
 * excess of the stall is dropped
 * When its lateness is taken
 * Then the first tick run started late by the whole stall, minus the one tick
 * it waited for anyway
 */
TEST(TickLoop, ReportsTheWholeStallWhenItsExcessIsDropped) {
  SimulatedClock clock;
  TickLoop loop{clock, [] {}};
  clock.advance(SIMULATION_TICK_DURATION * STALL_TICK_COUNT + HALF_TICK);
  loop.runDueTicks();

  const TickLoop::Lateness lateness = loop.takeLateness();

  EXPECT_EQ(lateness.maximum,
            SIMULATION_TICK_DURATION * (STALL_TICK_COUNT - 1) + HALF_TICK);
}

/**
 * Given a loop whose lateness was just taken
 * When it is taken again with no tick run in between
 * Then it reports no tick
 */
TEST(TickLoop, TakingTheLatenessStartsANewMeasurement) {
  SimulatedClock clock;
  TickLoop loop{clock, [] {}};
  clock.advance(SIMULATION_TICK_DURATION);
  loop.runDueTicks();
  loop.takeLateness();

  const TickLoop::Lateness lateness = loop.takeLateness();

  EXPECT_EQ(lateness.tickCount, 0U);
  EXPECT_EQ(lateness.total, Duration::zero());
}

/**
 * Given a loop first run sixty ticks and a half after it was created
 * When its stalls are taken
 * Then the stall dropped all the game time past the ticks caught up
 */
TEST(TickLoop, RecordsTheGameTimeAStallDropped) {
  SimulatedClock clock;
  TickLoop loop{clock, [] {}};
  stallFor(clock, loop, STALL_TICK_COUNT);

  const TickLoop::Stalls stalls = loop.takeStalls();

  EXPECT_EQ(stalls.totalDropped, droppedBy(STALL_TICK_COUNT));
}

/**
 * Given a loop whose clock jumped three ticks forward at once
 * When its stalls are taken
 * Then none is recorded: the missed ticks were caught up
 */
TEST(TickLoop, RecordsNoStallWhenTheMissedTicksAreCaughtUp) {
  SimulatedClock clock;
  TickLoop loop{clock, [] {}};
  clock.advance(SIMULATION_TICK_DURATION * MISSED_TICK_COUNT);
  loop.runDueTicks();

  const TickLoop::Stalls stalls = loop.takeStalls();

  EXPECT_EQ(stalls.count, 0U);
}

/**
 * Given a loop that went through a long stall, then a shorter one
 * When its stalls are taken
 * Then both are counted
 */
TEST(TickLoop, CountsEveryStall) {
  SimulatedClock clock;
  TickLoop loop{clock, [] {}};
  const std::array stallTickCounts{STALL_TICK_COUNT, SHORTER_STALL_TICK_COUNT};
  for (const int tickCount : stallTickCounts) {
    stallFor(clock, loop, tickCount);
  }

  const TickLoop::Stalls stalls = loop.takeStalls();

  EXPECT_EQ(stalls.count, stallTickCounts.size());
}

/**
 * Given a loop that went through a long stall, then a shorter one
 * When its stalls are taken
 * Then the game time they dropped adds up
 */
TEST(TickLoop, AddsUpTheGameTimeEveryStallDropped) {
  SimulatedClock clock;
  TickLoop loop{clock, [] {}};
  stallFor(clock, loop, STALL_TICK_COUNT);
  stallFor(clock, loop, SHORTER_STALL_TICK_COUNT);

  const TickLoop::Stalls stalls = loop.takeStalls();

  EXPECT_EQ(stalls.totalDropped,
            droppedBy(STALL_TICK_COUNT) + droppedBy(SHORTER_STALL_TICK_COUNT));
}

/**
 * Given a loop that went through a long stall, then a shorter one
 * When its stalls are taken
 * Then the longest is the game time the first one dropped
 */
TEST(TickLoop, KeepsTheLongestStall) {
  SimulatedClock clock;
  TickLoop loop{clock, [] {}};
  stallFor(clock, loop, STALL_TICK_COUNT);
  stallFor(clock, loop, SHORTER_STALL_TICK_COUNT);

  const TickLoop::Stalls stalls = loop.takeStalls();

  EXPECT_EQ(stalls.longestDropped, droppedBy(STALL_TICK_COUNT));
}

/**
 * Given a loop whose stalls were just taken
 * When they are taken again with no stall in between
 * Then none is reported
 */
TEST(TickLoop, TakingTheStallsStartsANewRecord) {
  SimulatedClock clock;
  TickLoop loop{clock, [] {}};
  stallFor(clock, loop, STALL_TICK_COUNT);
  loop.takeStalls();

  const TickLoop::Stalls stalls = loop.takeStalls();

  EXPECT_EQ(stalls.count, 0U);
  EXPECT_EQ(stalls.totalDropped, Duration::zero());
}

/**
 * Given a loop running on its own thread on the real clock, which ran a few
 * ticks
 * When a stop is requested
 * Then run() returns
 */
TEST(TickLoop, RunReturnsOnceAStopIsRequested) {
  const rtype::engine::SystemClock clock;
  std::atomic<int> tickCount{0};
  TickLoop loop{clock, [&tickCount] { ++tickCount; }};
  std::promise<void> returned;
  const std::future<void> runReturned = returned.get_future();
  std::jthread runner{[&loop, &returned](const std::stop_token& stop) {
    loop.run(stop);
    returned.set_value();
  }};
  const auto deadline = std::chrono::steady_clock::now() + THREAD_TIMEOUT;
  while (tickCount < TICKS_BEFORE_STOP &&
         std::chrono::steady_clock::now() < deadline) {
    std::this_thread::yield();
  }
  ASSERT_GE(tickCount, TICKS_BEFORE_STOP);

  runner.request_stop();

  EXPECT_EQ(runReturned.wait_for(THREAD_TIMEOUT), std::future_status::ready);
}
