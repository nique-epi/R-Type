#include <gtest/gtest.h>
#include <cstdint>
#include "ScriptWait.hpp"
#include "ScriptWaitTestConstants.hpp"

using rtype::engine::scriptWaitTicks;

/**
 * Given a wait of half a second
 * When it is turned into ticks
 * Then it lasts the 30 ticks of half a second
 */
TEST(ScriptWait, LastsTheTicksOfItsMilliseconds) {
  EXPECT_EQ(
      scriptWaitTicks(HALF_A_SECOND_IN_MILLISECONDS).value_or(std::uint32_t{0}),
      TICKS_IN_HALF_A_SECOND);
}

/**
 * Given a wait of a tick and a half
 * When it is turned into ticks
 * Then it is rounded to the nearest tick above, 2
 */
TEST(ScriptWait, RoundsHalfATickUp) {
  EXPECT_EQ(scriptWaitTicks(A_TICK_AND_A_HALF_IN_MILLISECONDS)
                .value_or(std::uint32_t{0}),
            TICKS_ROUNDED_UP_FROM_ONE_AND_A_HALF);
}

/**
 * Given a wait just under a tick and a half
 * When it is turned into ticks
 * Then it is rounded to the nearest tick under, 1
 */
TEST(ScriptWait, RoundsUnderHalfATickDown) {
  EXPECT_EQ(scriptWaitTicks(UNDER_A_TICK_AND_A_HALF_IN_MILLISECONDS)
                .value_or(std::uint32_t{0}),
            SHORTEST_WAIT_IN_TICKS);
}

/**
 * Given a wait of no time
 * When it is turned into ticks
 * Then it still lasts one tick
 */
TEST(ScriptWait, LastsOneTickAtLeast) {
  EXPECT_EQ(scriptWaitTicks(NO_WAIT_IN_MILLISECONDS).value_or(std::uint32_t{0}),
            SHORTEST_WAIT_IN_TICKS);
}

/**
 * Given a negative duration
 * When it is turned into ticks
 * Then it is no wait
 */
TEST(ScriptWait, RefusesANegativeDuration) {
  EXPECT_FALSE(scriptWaitTicks(NEGATIVE_MILLISECONDS).has_value());
}

/**
 * Given a duration without end
 * When it is turned into ticks
 * Then it is no wait
 */
TEST(ScriptWait, RefusesADurationThatIsNotFinite) {
  EXPECT_FALSE(scriptWaitTicks(ENDLESS_MILLISECONDS).has_value());
}
