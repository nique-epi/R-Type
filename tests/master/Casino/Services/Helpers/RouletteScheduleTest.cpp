#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include "MillisecondDate.hpp"
#include "RoulettePhase.hpp"
#include "RouletteSchedule.hpp"
#include "RouletteScheduleTestConstants.hpp"
#include "ScheduledRound.hpp"

using rtype::master::firstOpenRound;
using rtype::master::MillisecondDate;
using rtype::master::ScheduledRound;
using rtype::master::scheduledRoundAt;
using rtype::network::RoulettePhase;

/**
 * Given rounds of 20 s of bets, 6 s of spin and 5 s of result
 * When the clock reads the first millisecond of the round of the tests
 * Then that round opens its bets, and its phases end 20 s, 26 s and 31 s on
 */
TEST(RouletteSchedule, ARoundStartsWithItsBetsOpen) {
  const ScheduledRound scheduled =
      scheduledRoundAt(TEST_ROUND_START, TEST_DURATIONS);

  EXPECT_EQ(scheduled.round, TEST_ROUND);
  EXPECT_EQ(scheduled.phase, RoulettePhase::Betting);
  EXPECT_EQ(scheduled.closesAt, TEST_ROUND_START + BETS_CLOSE_AFTER);
  EXPECT_EQ(scheduled.resultAt, TEST_ROUND_START + RESULT_SHOWS_AFTER);
  EXPECT_EQ(scheduled.endsAt, TEST_ROUND_START + TEST_CYCLE);
}

/**
 * Given the round of the tests
 * When the clock reads one millisecond before, and the moment of, each end
 * of a phase
 * Then the phase changes at that moment, never a millisecond early
 */
TEST(RouletteSchedule, EachPhaseEndsAtItsDate) {
  const MillisecondDate start = TEST_ROUND_START;

  EXPECT_EQ(scheduledRoundAt(start + BETS_CLOSE_AFTER - ONE_MILLISECOND,
                             TEST_DURATIONS)
                .phase,
            RoulettePhase::Betting);
  EXPECT_EQ(scheduledRoundAt(start + BETS_CLOSE_AFTER, TEST_DURATIONS).phase,
            RoulettePhase::Spinning);
  EXPECT_EQ(scheduledRoundAt(start + RESULT_SHOWS_AFTER - ONE_MILLISECOND,
                             TEST_DURATIONS)
                .phase,
            RoulettePhase::Spinning);
  EXPECT_EQ(scheduledRoundAt(start + RESULT_SHOWS_AFTER, TEST_DURATIONS).phase,
            RoulettePhase::Result);
}

/**
 * Given the round of the tests
 * When the clock reads its last millisecond, then the next one
 * Then the last millisecond still shows its result, and the next one opens
 * the bets of the next round
 */
TEST(RouletteSchedule, TheNextRoundStartsWhereTheResultEnds) {
  const MillisecondDate end = TEST_ROUND_START + TEST_CYCLE;

  const ScheduledRound last =
      scheduledRoundAt(end - ONE_MILLISECOND, TEST_DURATIONS);
  const ScheduledRound next = scheduledRoundAt(end, TEST_DURATIONS);

  EXPECT_EQ(last.round, TEST_ROUND);
  EXPECT_EQ(last.phase, RoulettePhase::Result);
  EXPECT_EQ(next.round, TEST_ROUND + 1);
  EXPECT_EQ(next.phase, RoulettePhase::Betting);
  EXPECT_EQ(next.closesAt, end + BETS_CLOSE_AFTER);
}

/**
 * Given the round of the tests
 * When the clock reads each of its milliseconds
 * Then it stays the same round, 20000 of them betting, 6000 spinning and
 * 5000 showing the result
 */
TEST(RouletteSchedule, AWholeRoundSpendsEachDurationInItsPhase) {
  std::int64_t betting = 0;
  std::int64_t spinning = 0;
  std::int64_t result = 0;
  for (MillisecondDate now = TEST_ROUND_START;
       now < TEST_ROUND_START + TEST_CYCLE; now += ONE_MILLISECOND) {
    const ScheduledRound scheduled = scheduledRoundAt(now, TEST_DURATIONS);
    ASSERT_EQ(scheduled.round, TEST_ROUND);
    betting += scheduled.phase == RoulettePhase::Betting ? 1 : 0;
    spinning += scheduled.phase == RoulettePhase::Spinning ? 1 : 0;
    result += scheduled.phase == RoulettePhase::Result ? 1 : 0;
  }

  EXPECT_EQ(betting, TEST_DURATIONS.betting.count());
  EXPECT_EQ(spinning, TEST_DURATIONS.spinning.count());
  EXPECT_EQ(result, TEST_DURATIONS.result.count());
}

/**
 * Given the round of the tests
 * When its bets are open, then once they closed
 * Then the first round open to bets is its own, then the next one
 */
TEST(RouletteSchedule, TheFirstOpenRoundFollowsTheClosingOfTheBets) {
  EXPECT_EQ(firstOpenRound(scheduledRoundAt(TEST_ROUND_START, TEST_DURATIONS)),
            TEST_ROUND);
  EXPECT_EQ(firstOpenRound(scheduledRoundAt(TEST_ROUND_START + BETS_CLOSE_AFTER,
                                            TEST_DURATIONS)),
            TEST_ROUND + 1);
}
