#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>
#include "FlightLogFixtures.hpp"
#include "FlightLogSchedule.hpp"
#include "FlightLogTestConstants.hpp"
#include "FlightLogTextConstants.hpp"
#include "FlightLogTimeline.hpp"
#include "FlightLogTimelineTestConstants.hpp"
#include "FlightLogTimingConstants.hpp"
#include "SoundConstants.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::client::CLICK_SOUND_ID;
using rtype::client::cuesBetween;
using rtype::client::DOWN_DELAY;
using rtype::client::DOWN_LETTER_TIME;
using rtype::client::DOWN_TEXT;
using rtype::client::FlightLogSchedule;
using rtype::client::HEAVY_IMPACT_SOUND_ID;
using rtype::client::RANKING_DELAY;
using rtype::client::testing::DESIGN_READY;
using rtype::client::testing::DESIGN_ROW_STEP;
using rtype::client::testing::FOUR_PILOTS;
using rtype::client::testing::THREE_PILOTS;
using rtype::engine::testing::ONE_SECOND;
using ::testing::Contains;
using ::testing::Not;

/**
 * Given the game of the design system: three pilots, a taunt of 26 letters
 * When its flight log is scheduled
 * Then the choices take input at 7.75 s, as the design says
 */
TEST(FlightLogTimeline, TheChoicesComeWhenTheDesignSays) {
  EXPECT_EQ(designScheduleOf(THREE_PILOTS).ready, DESIGN_READY);
}

/**
 * Given the left page of the open book
 * When FINAL RANKING starts
 * Then ALL PILOTS DOWN is already written whole
 */
TEST(FlightLogTimeline, TheRankingWaitsForAllPilotsDown) {
  const auto downWritten =
      DOWN_DELAY +
      (DOWN_LETTER_TIME *
       static_cast<rtype::engine::Duration::rep>(DOWN_TEXT.size()));
  EXPECT_LE(downWritten, RANKING_DELAY);
}

/**
 * Given a game of three pilots, then of four
 * When both flight logs are scheduled
 * Then the fourth pilot adds one row step, 0.4 s
 */
TEST(FlightLogTimeline, EachPilotAddsOneRowStep) {
  EXPECT_EQ(designScheduleOf(FOUR_PILOTS).ready -
                designScheduleOf(THREE_PILOTS).ready,
            DESIGN_ROW_STEP);
}

/**
 * Given the first second, then the next one
 * When their sounds are asked
 * Then the landing plays its impact in the first only
 */
TEST(FlightLogTimeline, TheLandingPlaysItsImpactOnce) {
  const FlightLogSchedule schedule = designScheduleOf(THREE_PILOTS);
  const std::string_view impact = HEAVY_IMPACT_SOUND_ID;

  EXPECT_THAT(cuesBetween(schedule, {}, ONE_SECOND), Contains(impact));
  EXPECT_THAT(cuesBetween(schedule, ONE_SECOND, ONE_SECOND * 2),
              Not(Contains(impact)));
}

/**
 * Given three rows
 * When the sounds from just before the first row to the end are asked
 * Then a click plays per row
 */
TEST(FlightLogTimeline, EveryRowPlaysAClick) {
  const FlightLogSchedule schedule = designScheduleOf(THREE_PILOTS);
  const std::vector<std::string_view> cues = cuesBetween(
      schedule, schedule.rowsStart - ONE_NANOSECOND, schedule.ready);
  const std::string_view click = CLICK_SOUND_ID;

  EXPECT_EQ(static_cast<std::size_t>(std::ranges::count(cues, click)),
            THREE_PILOTS);
}
