#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"

namespace {

using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::engine::wholeTicksIn;

/**
 * @brief The intervals the protocol page gives, and the ticks they hold at 60
 * ticks per second.
 */
constexpr std::chrono::milliseconds HALF_A_SECOND{500};
constexpr std::size_t TICKS_IN_HALF_A_SECOND = 30;
constexpr std::chrono::seconds ONE_SECOND{1};
constexpr std::size_t TICKS_IN_ONE_SECOND = 60;
constexpr std::chrono::seconds FIVE_SECONDS{5};
constexpr std::size_t TICKS_IN_FIVE_SECONDS = 300;

}  // namespace

/**
 * Given the intervals of the protocol page: 500 ms, 1 s and 5 s
 * When each is counted in whole ticks
 * Then they hold 30, 60 and 300 ticks, at 60 ticks per second
 */
TEST(TimeConstants, CountsTheIntervalsOfTheProtocolInWholeTicks) {
  EXPECT_EQ(wholeTicksIn(HALF_A_SECOND), TICKS_IN_HALF_A_SECOND);
  EXPECT_EQ(wholeTicksIn(ONE_SECOND), TICKS_IN_ONE_SECOND);
  EXPECT_EQ(wholeTicksIn(FIVE_SECONDS), TICKS_IN_FIVE_SECONDS);
}

/**
 * Given a duration one nanosecond short of a tick, and one of exactly a tick
 * When each is counted in whole ticks
 * Then the first holds no tick and the second one: the count rounds down
 */
TEST(TimeConstants, RoundsDownToTheLastWholeTick) {
  EXPECT_EQ(wholeTicksIn(SIMULATION_TICK_DURATION - ONE_NANOSECOND), 0U);
  EXPECT_EQ(wholeTicksIn(SIMULATION_TICK_DURATION), 1U);
}
