#include <gtest/gtest.h>
#include <span>
#include "ScriptBudgetBenchTestConstants.hpp"
#include "TickFigures.hpp"
#include "TickSample.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Duration;
using rtype::tools::figuresOf;
using rtype::tools::TickFigures;
using rtype::tools::TickSample;
using rtype::tools::testing::COLLISION_SHARE_OF_FOUR_TICKS;
using rtype::tools::testing::COLLISIONS_OF_FOUR_TICKS;
using rtype::tools::testing::FOUR_MILLISECONDS;
using rtype::tools::testing::FOUR_TICKS_OUT_OF_ORDER;
using rtype::tools::testing::ONE_MILLISECOND;
using rtype::tools::testing::SHARE_OF_FOUR_TICKS;
using rtype::tools::testing::SHARE_TOLERANCE;
using rtype::tools::testing::THREE_MILLISECONDS;
using rtype::tools::testing::TWO_MILLISECONDS;

/**
 * Given four ticks out of order
 * When their figures are computed
 * Then the median is the lower of the two middle ticks, and the worst the
 * slowest
 */
TEST(TickFigures, RanksTheTicksInTheirOrderOfDuration) {
  const TickFigures figures =
      figuresOf(FOUR_TICKS_OUT_OF_ORDER, FOUR_MILLISECONDS);

  EXPECT_EQ(figures.tick.median, TWO_MILLISECONDS);
  EXPECT_EQ(figures.tick.high, FOUR_MILLISECONDS);
  EXPECT_EQ(figures.tick.worst, FOUR_MILLISECONDS);
}

/**
 * Given four ticks whose scripts take 6 ms of 10 ms in all
 * When their figures are computed
 * Then the scripts have their own spread, and a share of 60 percent
 */
TEST(TickFigures, SpreadsTheScriptsApartFromTheTicks) {
  const TickFigures figures =
      figuresOf(FOUR_TICKS_OUT_OF_ORDER, FOUR_MILLISECONDS);

  EXPECT_EQ(figures.scripts.median, ONE_MILLISECOND);
  EXPECT_EQ(figures.scripts.worst, THREE_MILLISECONDS);
  EXPECT_NEAR(figures.scriptShare, SHARE_OF_FOUR_TICKS, SHARE_TOLERANCE);
}

/**
 * Given four ticks, one of which takes exactly the budget
 * When their figures are computed
 * Then only the tick longer than the budget counts as over it
 */
TEST(TickFigures, ATickOfExactlyTheBudgetIsWithinIt) {
  const TickFigures figures =
      figuresOf(FOUR_TICKS_OUT_OF_ORDER, THREE_MILLISECONDS);

  EXPECT_EQ(figures.ticksOverBudget, 1U);
}

/**
 * Given no tick
 * When the figures are computed
 * Then they are all zero
 */
TEST(TickFigures, NoTickGivesFiguresOfZero) {
  const TickFigures figures =
      figuresOf(std::span<const TickSample>{}, ONE_MILLISECOND);

  EXPECT_EQ(figures.tick.worst, Duration::zero());
  EXPECT_EQ(figures.scriptShare, 0.0);
  EXPECT_EQ(figures.ticksOverBudget, 0U);
}

/**
 * Given a single tick
 * When its figures are computed
 * Then every rank is that tick
 */
TEST(TickFigures, EveryRankOfASingleTickIsThatTick) {
  const TickSample single{.whole = ONE_MILLISECOND, .scripts = ONE_MILLISECOND};

  const TickFigures figures =
      figuresOf(std::span{&single, 1}, FOUR_MILLISECONDS);

  EXPECT_EQ(figures.tick.median, ONE_MILLISECOND);
  EXPECT_EQ(figures.tick.high, ONE_MILLISECOND);
  EXPECT_EQ(figures.tick.worst, ONE_MILLISECOND);
}

/**
 * Given four ticks whose collisions take 2 ms of 10 ms in all, and find 6
 * When their figures are computed
 * Then the collisions have their own spread, a share of 20 percent, and 6
 * collisions found
 */
TEST(TickFigures, ReportsTheShareAndCountOfTheCollisions) {
  const TickFigures figures =
      figuresOf(FOUR_TICKS_OUT_OF_ORDER, FOUR_MILLISECONDS);

  EXPECT_EQ(figures.collisions.worst, ONE_MILLISECOND);
  EXPECT_NEAR(figures.collisionShare, COLLISION_SHARE_OF_FOUR_TICKS,
              SHARE_TOLERANCE);
  EXPECT_EQ(figures.collisionsFound, COLLISIONS_OF_FOUR_TICKS);
}
