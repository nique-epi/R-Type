#include <gtest/gtest.h>
#include <chrono>
#include <stop_token>
#include <thread>
#include "InterruptibleSleep.hpp"
#include "InterruptibleSleepTestConstants.hpp"

using rtype::concurrency::sleepUnlessStopped;
using rtype::concurrency::testing::LONG_SLEEP;
using rtype::concurrency::testing::SHORT_SLEEP;

/**
 * Given a thread that is never asked to stop
 * When it sleeps a short time
 * Then it wakes only once that whole time passed
 */
TEST(InterruptibleSleep, SleepsTheWholeTimeWhenNoStopIsRequested) {
  const auto before = std::chrono::steady_clock::now();

  sleepUnlessStopped(std::stop_token{}, SHORT_SLEEP);

  EXPECT_GE(std::chrono::steady_clock::now() - before, SHORT_SLEEP);
}

/**
 * Given a thread sleeping a long time
 * When another thread asks it to stop
 * Then it wakes before the end of its sleep
 */
TEST(InterruptibleSleep, AStopRequestWakesTheThreadThatSleeps) {
  std::stop_source stop;
  const auto before = std::chrono::steady_clock::now();
  const std::jthread stopper{[&stop] {
    std::this_thread::sleep_for(SHORT_SLEEP);
    stop.request_stop();
  }};

  sleepUnlessStopped(stop.get_token(), LONG_SLEEP);

  EXPECT_LT(std::chrono::steady_clock::now() - before, LONG_SLEEP);
}

/**
 * Given a stop already requested
 * When the thread starts a long sleep
 * Then it returns before the end of that sleep
 */
TEST(InterruptibleSleep, AStopRequestedBeforehandEndsTheSleepAtOnce) {
  std::stop_source stop;
  stop.request_stop();
  const auto before = std::chrono::steady_clock::now();

  sleepUnlessStopped(stop.get_token(), LONG_SLEEP);

  EXPECT_LT(std::chrono::steady_clock::now() - before, LONG_SLEEP);
}
