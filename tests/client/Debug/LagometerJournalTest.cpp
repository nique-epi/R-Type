#include <gtest/gtest.h>
#include <chrono>
#include <optional>
#include <tuple>
#include "LagometerFixtures.hpp"
#include "LagometerJournal.hpp"
#include "LagometerJournalFigures.hpp"
#include "LagometerJournalTestConstants.hpp"

namespace rtype::client {

using testing::AT_THE_SLOW_FRAME;
using testing::delayedReading;
using testing::EIGHT_POINT_FOUR_TICKS_BEHIND;
using testing::frameReading;
using testing::JOURNAL_FIRST_FRAME;
using testing::JOURNAL_LAST_FRAME;
using testing::JOURNAL_PERIOD_FIGURES;
using testing::JOURNAL_PERIOD_IN_THE_TESTS;
using testing::JUST_SHORT_OF_THE_PERIOD;
using testing::LARGEST_DELAY_OF_THE_PERIOD;
using testing::PAST_THE_SLOW_FRAME;
using testing::SEVEN_TICKS_BEHIND;
using testing::SIX_TICKS_BEHIND;
using testing::SMALLEST_DELAY_OF_THE_PERIOD;

/**
 * Given a journal that read the first frame of a game
 * When frames add up to 1 ms short of 10 s
 * Then it gives nothing: the period has not ended
 */
TEST(LagometerJournal, GivesNothingBeforeThePeriodEnds) {
  LagometerJournal journal;
  std::ignore = journal.update(JOURNAL_FIRST_FRAME);

  const auto figures = journal.update(
      delayedReading(JUST_SHORT_OF_THE_PERIOD, SIX_TICKS_BEHIND));

  EXPECT_FALSE(figures.has_value());
}

/**
 * Given a journal whose first frame read 10 datagrams accepted, 1 missed and
 * 12 frames past the newest state
 * When the frame that ends the period reads 300 accepted, 3 missed, 66 frames
 * past the newest state and a ping of 85 ms
 * Then the period gives 290 accepted, 2 missed, 2 frames, 54 past the newest
 * state and that ping, over 10 s
 */
TEST(LagometerJournal, GivesWhatThePeriodCounted) {
  LagometerJournal journal;
  std::ignore = journal.update(JOURNAL_FIRST_FRAME);

  const auto figures = journal.update(JOURNAL_LAST_FRAME);

  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(LagometerJournalFigures{}),
            JOURNAL_PERIOD_FIGURES);
}

/**
 * Given a period whose frames drew 6, then 8.4, then 7 ticks behind the
 * server
 * When it ends
 * Then its delay ranges from 100 ms to 140 ms
 */
TEST(LagometerJournal, GivesTheRangeOfTheDelay) {
  LagometerJournal journal;
  std::ignore = journal.update(
      delayedReading(std::chrono::milliseconds::zero(), SIX_TICKS_BEHIND));
  std::ignore = journal.update(
      delayedReading(JUST_SHORT_OF_THE_PERIOD, EIGHT_POINT_FOUR_TICKS_BEHIND));

  const auto figures = journal.update(
      delayedReading(std::chrono::milliseconds{1}, SEVEN_TICKS_BEHIND));

  ASSERT_TRUE(figures.has_value());
  const LagometerJournalFigures period =
      figures.value_or(LagometerJournalFigures{});
  EXPECT_EQ(period.smallestDelay, SMALLEST_DELAY_OF_THE_PERIOD);
  EXPECT_EQ(period.largestDelay, LARGEST_DELAY_OF_THE_PERIOD);
}

/**
 * Given a journal reading frames in which no game drew, as on the title
 * When 10 s of them pass
 * Then it gives nothing: there is no display to measure
 */
TEST(LagometerJournal, GivesNothingForAPeriodWithoutAGame) {
  LagometerJournal journal;
  std::ignore = journal.update(frameReading(std::chrono::milliseconds::zero()));

  const auto figures =
      journal.update(frameReading(JOURNAL_PERIOD_IN_THE_TESTS));

  EXPECT_FALSE(figures.has_value());
}

/**
 * Given a journal whose period just ended
 * When frames add up to 1 ms short of the next 10 s
 * Then it gives nothing: a new period started with the frame that ended the
 * last one
 */
TEST(LagometerJournal, StartsTheNextPeriodWhereTheLastEnded) {
  LagometerJournal journal;
  std::ignore = journal.update(JOURNAL_FIRST_FRAME);
  std::ignore = journal.update(JOURNAL_LAST_FRAME);

  const auto figures = journal.update(
      delayedReading(JUST_SHORT_OF_THE_PERIOD, SIX_TICKS_BEHIND));

  EXPECT_FALSE(figures.has_value());
}

/**
 * Given a period in which the game drew, then a period in which it drew no
 * more, as after the end of a game
 * When the second period ends
 * Then it gives nothing: the delays of the first period are not carried over
 */
TEST(LagometerJournal, ForgetsTheDelaysOfTheLastPeriod) {
  LagometerJournal journal;
  std::ignore = journal.update(JOURNAL_FIRST_FRAME);
  std::ignore = journal.update(JOURNAL_LAST_FRAME);

  const auto figures =
      journal.update(frameReading(JOURNAL_PERIOD_IN_THE_TESTS));

  EXPECT_FALSE(figures.has_value());
}

/**
 * Given a period with a frame of 25 ms and a frame of 26 ms
 * When a frame of 9.999 s ends it
 * Then two frames are slow, the 26 ms one and the last, the 25 ms one not,
 * and the longest is the last
 */
TEST(LagometerJournal, CountsTheFramesPastTheSlowFrame) {
  LagometerJournal journal;
  std::ignore = journal.update(
      delayedReading(std::chrono::milliseconds::zero(), SIX_TICKS_BEHIND));
  std::ignore =
      journal.update(delayedReading(AT_THE_SLOW_FRAME, SIX_TICKS_BEHIND));
  std::ignore =
      journal.update(delayedReading(PAST_THE_SLOW_FRAME, SIX_TICKS_BEHIND));

  const auto figures = journal.update(
      delayedReading(JUST_SHORT_OF_THE_PERIOD, SIX_TICKS_BEHIND));

  ASSERT_TRUE(figures.has_value());
  const LagometerJournalFigures period =
      figures.value_or(LagometerJournalFigures{});
  EXPECT_EQ(period.slowFrames, 2U);
  EXPECT_EQ(period.longestFrame, JUST_SHORT_OF_THE_PERIOD);
}

}  // namespace rtype::client
