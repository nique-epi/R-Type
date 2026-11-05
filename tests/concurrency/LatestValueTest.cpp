#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stop_token>
#include <thread>
#include <vector>
#include "LatestValue.hpp"
#include "LatestValueFixtures.hpp"
#include "LatestValueTestConstants.hpp"

using rtype::concurrency::LatestValue;
using rtype::concurrency::testing::FIRST_REVISION;
using rtype::concurrency::testing::LONG_WAIT;
using rtype::concurrency::testing::LONGEST_VALUE;
using rtype::concurrency::testing::NEWER_VALUE;
using rtype::concurrency::testing::NO_REVISION;
using rtype::concurrency::testing::OLDER_VALUE;
using rtype::concurrency::testing::SHORT_WAIT;
using rtype::concurrency::testing::TIMES_READ;
using rtype::concurrency::testing::valueCounting;
using rtype::concurrency::testing::VALUES_PUBLISHED;

/**
 * Given nothing was published
 * When the value is read
 * Then there is none, at revision 0
 */
TEST(LatestValue, HoldsNoValueBeforeTheFirstOne) {
  const LatestValue<std::size_t> latest;

  EXPECT_FALSE(latest.value().has_value());
  EXPECT_EQ(latest.revision(), NO_REVISION);
}

/**
 * Given two values were published, one after the other
 * When the value is read
 * Then it is the newer one
 */
TEST(LatestValue, TheNewerValueReplacesTheOlderOne) {
  LatestValue<std::size_t> latest;
  latest.publish(OLDER_VALUE);
  latest.publish(NEWER_VALUE);

  EXPECT_EQ(latest.value().value_or(0), NEWER_VALUE);
}

/**
 * Given a value was published
 * When it is read twice, as two readers at different rates do
 * Then the second read finds the same value
 */
TEST(LatestValue, ReadingLeavesTheValueInPlace) {
  LatestValue<std::size_t> latest;
  latest.publish(OLDER_VALUE);

  [[maybe_unused]] const std::optional<std::size_t> first = latest.value();

  EXPECT_EQ(latest.value().value_or(0), OLDER_VALUE);
}

/**
 * Given a thread that keeps publishing values
 * When another thread reads meanwhile
 * Then each value it reads is whole: every element comes from one publication
 */
TEST(LatestValue, AReaderOnAnotherThreadOnlySeesWholeValues) {
  LatestValue<std::vector<std::size_t>> latest;
  const std::jthread writer{[&latest] {
    for (std::size_t count = 0; count < VALUES_PUBLISHED; ++count) {
      latest.publish(valueCounting(count));
    }
  }};

  for (std::size_t read = 0; read < TIMES_READ; ++read) {
    const std::vector<std::size_t> value =
        latest.value().value_or(std::vector<std::size_t>{});
    for (const std::size_t element : value) {
      ASSERT_EQ(element, value.front());
    }
    if (!value.empty()) {
      ASSERT_EQ(value.size(), value.front() % LONGEST_VALUE);
    }
  }
}

/**
 * Given a value newer than the revision a reader saw
 * When the reader waits for a newer one
 * Then it returns at once with the newer revision
 */
TEST(LatestValue, AWaitForANewerValueReturnsAtOnceWhenOneIsThere) {
  LatestValue<std::size_t> latest;
  latest.publish(OLDER_VALUE);

  EXPECT_EQ(latest.waitForNewer(NO_REVISION, LONG_WAIT, std::stop_token{}),
            FIRST_REVISION);
}

/**
 * Given nothing newer than the revision a reader saw
 * When the reader waits a short time
 * Then it returns once that time passed, with the revision it saw
 */
TEST(LatestValue, AWaitWithNothingNewEndsWithItsTimeout) {
  LatestValue<std::size_t> latest;
  latest.publish(OLDER_VALUE);

  EXPECT_EQ(latest.waitForNewer(FIRST_REVISION, SHORT_WAIT, std::stop_token{}),
            FIRST_REVISION);
}

/**
 * Given a reader waiting a long time for a newer value
 * When another thread publishes one
 * Then the reader wakes with the newer revision before its timeout
 */
TEST(LatestValue, APublicationWakesTheReaderThatWaits) {
  LatestValue<std::size_t> latest;
  const auto before = std::chrono::steady_clock::now();
  const std::jthread writer{[&latest] {
    std::this_thread::sleep_for(SHORT_WAIT);
    latest.publish(NEWER_VALUE);
  }};

  const std::uint64_t revision =
      latest.waitForNewer(NO_REVISION, LONG_WAIT, std::stop_token{});

  EXPECT_EQ(revision, FIRST_REVISION);
  EXPECT_LT(std::chrono::steady_clock::now() - before, LONG_WAIT);
}

/**
 * Given a reader waiting a long time for a newer value
 * When its thread is asked to stop
 * Then the reader wakes before its timeout, with nothing newer
 */
TEST(LatestValue, AStopRequestWakesTheReaderThatWaits) {
  LatestValue<std::size_t> latest;
  std::stop_source stop;
  const auto before = std::chrono::steady_clock::now();
  const std::jthread stopper{[&stop] {
    std::this_thread::sleep_for(SHORT_WAIT);
    stop.request_stop();
  }};

  const std::uint64_t revision =
      latest.waitForNewer(NO_REVISION, LONG_WAIT, stop.get_token());

  EXPECT_EQ(revision, NO_REVISION);
  EXPECT_LT(std::chrono::steady_clock::now() - before, LONG_WAIT);
}
