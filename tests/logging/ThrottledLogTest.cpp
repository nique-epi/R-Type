#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <string>
#include <vector>
#include "DebugJournal.hpp"
#include "ThrottledLog.hpp"
#include "ThrottledLogTestConstants.hpp"
#include "TimeTestConstants.hpp"

namespace rtype::logging {
namespace {

using testing::EVENT_TEXT;
using testing::FIRST_EVENT_TIME;
using testing::FLOODED_EVENT_COUNT;
using testing::HALF_INTERVAL;
using testing::THROTTLED_INTERVAL;
using testing::THROTTLED_MODULE;

/**
 * @returns The end of a line that stands for @p count events.
 */
std::string countEnding(std::size_t count) {
  return " (" + std::to_string(count) + " since the previous line)";
}

/** @brief A throttled log, with the log written to a journal the test reads. */
class ThrottledLogTest : public ::testing::Test {
 protected:
  void eventAt(std::chrono::nanoseconds now) { log_.debug(now, EVENT_TEXT); }

  /** @returns The lines the throttled log wrote, oldest first. */
  [[nodiscard]] std::vector<std::string> lines() const {
    return journal_.linesHolding(EVENT_TEXT);
  }

 private:
  DebugJournal journal_;
  ThrottledLog log_{std::string{THROTTLED_MODULE}, THROTTLED_INTERVAL};
};

/**
 * Given a throttled log that wrote nothing
 * When one event happens
 * Then the journal holds one line with the text of the event, standing for
 * that one event
 */
TEST_F(ThrottledLogTest, WritesALineForTheFirstEvent) {
  eventAt(FIRST_EVENT_TIME);

  const std::vector<std::string> written = lines();
  ASSERT_EQ(written.size(), 1U);
  EXPECT_NE(written.front().find(std::string{EVENT_TEXT} + countEnding(1)),
            std::string::npos);
}

/**
 * Given a throttled log that wrote a line for an event
 * When another event happens one nanosecond before the interval is over
 * Then no line is written for it
 */
TEST_F(ThrottledLogTest, WritesNoLineBeforeTheIntervalIsOver) {
  eventAt(FIRST_EVENT_TIME);

  eventAt(FIRST_EVENT_TIME + THROTTLED_INTERVAL - ONE_NANOSECOND);

  EXPECT_EQ(lines().size(), 1U);
}

/**
 * Given a throttled log that wrote a line for an event
 * When another event happens exactly one interval later
 * Then a second line is written for it
 */
TEST_F(ThrottledLogTest, WritesTheNextLineOnceTheIntervalIsOver) {
  eventAt(FIRST_EVENT_TIME);

  eventAt(FIRST_EVENT_TIME + THROTTLED_INTERVAL);

  EXPECT_EQ(lines().size(), 2U);
}

/**
 * Given a throttled log that wrote a line, then counted events without a line
 * When an event happens once the interval is over
 * Then its line stands for the events without a line and for itself
 */
TEST_F(ThrottledLogTest, CountsTheEventsWithoutALineInTheNextLine) {
  eventAt(FIRST_EVENT_TIME);
  for (std::size_t event = 0; event < FLOODED_EVENT_COUNT; ++event) {
    eventAt(FIRST_EVENT_TIME + THROTTLED_INTERVAL - ONE_NANOSECOND);
  }

  eventAt(FIRST_EVENT_TIME + THROTTLED_INTERVAL);

  const std::vector<std::string> written = lines();
  ASSERT_EQ(written.size(), 2U);
  EXPECT_TRUE(written.back().ends_with(countEnding(FLOODED_EVENT_COUNT + 1)));
}

/**
 * Given a throttled log that wrote a line for an event, and nothing since
 * When an event happens once the interval is over
 * Then its line stands for that one event: the count starts again after each
 * line
 */
TEST_F(ThrottledLogTest, CountsAgainFromZeroAfterEachLine) {
  eventAt(FIRST_EVENT_TIME);

  eventAt(FIRST_EVENT_TIME + THROTTLED_INTERVAL);

  const std::vector<std::string> written = lines();
  ASSERT_EQ(written.size(), 2U);
  EXPECT_TRUE(written.back().ends_with(countEnding(1)));
}

/**
 * Given a throttled log that wrote a line for a first event, then one for an
 * event an interval and a half later
 * When an event happens two intervals after the first
 * Then no line is written: the interval runs from the previous line, so two
 * lines are never closer than one interval
 */
TEST_F(ThrottledLogTest, MeasuresTheIntervalFromThePreviousLine) {
  eventAt(FIRST_EVENT_TIME);
  eventAt(FIRST_EVENT_TIME + THROTTLED_INTERVAL + HALF_INTERVAL);

  eventAt(FIRST_EVENT_TIME + THROTTLED_INTERVAL + THROTTLED_INTERVAL);

  EXPECT_EQ(lines().size(), 2U);
}

}  // namespace
}  // namespace rtype::logging
