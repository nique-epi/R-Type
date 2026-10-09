#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include "SkyEventConstants.hpp"
#include "SkyEventScheduler.hpp"

using rtype::client::FIRST_SKY_EVENT_DELAY;
using rtype::client::MAXIMUM_SKY_EVENTS;
using rtype::client::SkyEventScheduler;

namespace {

constexpr std::uint32_t SEED = 2026;
constexpr float FRAME_SECONDS = 0.01F;
constexpr float A_TENTH_OF_A_SECOND = 0.1F;
constexpr float ONE_HOUR = 3600.0F;

int framesIn(float seconds) {
  return static_cast<int>(std::lround(seconds / FRAME_SECONDS));
}

void advanceFor(SkyEventScheduler& scheduler, float seconds) {
  for (int frame = 0; frame < framesIn(seconds); ++frame) {
    scheduler.advance(FRAME_SECONDS);
  }
}

}  // namespace

/**
 * Given a new scheduler
 * When less time than the first delay passes
 * Then no event is shown
 */
TEST(SkyEventScheduler, ShowsNothingBeforeTheFirstDelay) {
  SkyEventScheduler scheduler(SEED);

  advanceFor(scheduler, FIRST_SKY_EVENT_DELAY - A_TENTH_OF_A_SECOND);

  EXPECT_EQ(scheduler.eventCount(), 0U);
}

/**
 * Given a new scheduler
 * When a tenth of a second more than the first delay passes, less than the
 * shortest gap and than the shortest event
 * Then exactly one event is shown
 */
TEST(SkyEventScheduler, StartsOneEventOnceTheFirstDelayHasPassed) {
  SkyEventScheduler scheduler(SEED);

  advanceFor(scheduler, FIRST_SKY_EVENT_DELAY + A_TENTH_OF_A_SECOND);

  EXPECT_EQ(scheduler.eventCount(), 1U);
}

/**
 * Given a scheduler
 * When an hour passes, long enough for more than four events to overlap if
 * nothing capped them
 * Then the number of events shown at once reaches the maximum and never
 * exceeds it
 */
TEST(SkyEventScheduler, ShowsAtMostTheMaximumAtOnce) {
  SkyEventScheduler scheduler(SEED);
  std::size_t mostShown = 0;

  for (int frame = 0; frame < framesIn(ONE_HOUR); ++frame) {
    scheduler.advance(FRAME_SECONDS);
    mostShown = std::max(mostShown, scheduler.eventCount());
  }

  EXPECT_EQ(mostShown, MAXIMUM_SKY_EVENTS);
}
