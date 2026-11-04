#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <string>
#include <vector>
#include "LogLineTestConstants.hpp"
#include "ThrottledLogFixture.hpp"
#include "ThrottledLogTestConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::logging::testing::EVENT_TEXT;
using rtype::logging::testing::FIRST_EVENT_TIME;
using rtype::logging::testing::FLOODED_EVENT_COUNT;
using rtype::logging::testing::HALF_INTERVAL;
using rtype::logging::testing::THROTTLED_INTERVAL;
using rtype::logging::testing::ThrottledLogFixture;

/**
 * Given a throttled log that wrote nothing
 * When one error event happens
 * Then the journal holds one line at the error level with the text of the
 * event
 */
TEST_F(ThrottledLogFixture, WritesAnErrorEventAtTheErrorLevel) {
  errorAt(FIRST_EVENT_TIME);

  const std::vector<std::string> written = lines();
  ASSERT_EQ(written.size(), 1U);
  EXPECT_NE(written.front().find(ERROR_LINE_MARK), std::string::npos);
}

/**
 * Given a throttled log that wrote nothing
 * When one event happens
 * Then the journal holds one line with the text of the event, standing for
 * that one event
 */
TEST_F(ThrottledLogFixture, WritesALineForTheFirstEvent) {
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
TEST_F(ThrottledLogFixture, WritesNoLineBeforeTheIntervalIsOver) {
  eventAt(FIRST_EVENT_TIME);

  eventAt(FIRST_EVENT_TIME + THROTTLED_INTERVAL - ONE_NANOSECOND);

  EXPECT_EQ(lines().size(), 1U);
}

/**
 * Given a throttled log that wrote a line for an event
 * When another event happens exactly one interval later
 * Then a second line is written for it
 */
TEST_F(ThrottledLogFixture, WritesTheNextLineOnceTheIntervalIsOver) {
  eventAt(FIRST_EVENT_TIME);

  eventAt(FIRST_EVENT_TIME + THROTTLED_INTERVAL);

  EXPECT_EQ(lines().size(), 2U);
}

/**
 * Given a throttled log that wrote a line, then counted events without a line
 * When an event happens once the interval is over
 * Then its line stands for the events without a line and for itself
 */
TEST_F(ThrottledLogFixture, CountsTheEventsWithoutALineInTheNextLine) {
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
TEST_F(ThrottledLogFixture, CountsAgainFromZeroAfterEachLine) {
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
TEST_F(ThrottledLogFixture, MeasuresTheIntervalFromThePreviousLine) {
  eventAt(FIRST_EVENT_TIME);
  eventAt(FIRST_EVENT_TIME + THROTTLED_INTERVAL + HALF_INTERVAL);

  eventAt(FIRST_EVENT_TIME + THROTTLED_INTERVAL + THROTTLED_INTERVAL);

  EXPECT_EQ(lines().size(), 2U);
}
