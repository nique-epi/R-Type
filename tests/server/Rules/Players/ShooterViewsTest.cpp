#include <gtest/gtest.h>
#include "ProtocolConstants.hpp"
#include "ShooterViewsFixture.hpp"
#include "ShooterViewsTestConstants.hpp"

using rtype::network::NO_SHOWN_TICK;
using rtype::server::testing::FIVE_TICKS_BEHIND;
using rtype::server::testing::PRESENT_TICK;
using rtype::server::testing::ShooterViewsFixture;
using rtype::server::testing::TICK_FIVE_BEFORE;

/**
 * Given a missile whose player's client showed tick 95 when it sent the input
 * applied last
 * When the views are given at tick 100
 * Then the missile is tested five ticks behind the present
 */
TEST_F(ShooterViewsFixture, GivesAMissileTheTicksItsPlayerSawBehind) {
  shooter().appliedShownTick = TICK_FIVE_BEFORE;

  giveViewsAt(PRESENT_TICK);

  EXPECT_EQ(missileDelay().value_or(0), FIVE_TICKS_BEHIND);
}

/**
 * Given a missile whose player's client showed no state yet
 * When the views are given at tick 100
 * Then the missile is tested in the present
 */
TEST_F(ShooterViewsFixture, TestsInThePresentBeforeTheFirstState) {
  shooter().appliedShownTick = NO_SHOWN_TICK;

  giveViewsAt(PRESENT_TICK);

  ASSERT_TRUE(missileDelay().has_value());
  EXPECT_EQ(missileDelay().value_or(FIVE_TICKS_BEHIND), 0U);
}

/**
 * Given a missile whose player's client claims to show tick 100, the present
 * When the views are given at tick 100
 * Then the missile is tested in the present, never ahead of it
 */
TEST_F(ShooterViewsFixture, NeverLooksAheadOfThePresent) {
  shooter().appliedShownTick = PRESENT_TICK;

  giveViewsAt(PRESENT_TICK);

  ASSERT_TRUE(missileDelay().has_value());
  EXPECT_EQ(missileDelay().value_or(FIVE_TICKS_BEHIND), 0U);
}
