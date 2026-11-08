#include <gtest/gtest.h>
#include <cstddef>
#include "FavorTheShooter.hpp"
#include "HitArbitrationFixture.hpp"
#include "ShooterViewsTestConstants.hpp"

using rtype::server::FavorTheShooter;
using rtype::server::testing::FIVE_TICKS_BEHIND;
using rtype::server::testing::HitArbitrationFixture;
using rtype::server::testing::SHOOTER_REWIND_IN_TICKS;

/**
 * Given the ship of a player whose client showed tick 95, and a missile it
 * fired
 * When the arbitration between players gives its views at tick 100
 * Then the ship gets no view: it is tested in the present of the server
 */
TEST_F(HitArbitrationFixture, TheShooterArbitrationTestsTheShipInThePresent) {
  giveViewsOf(FavorTheShooter{});

  EXPECT_FALSE(shipDelay().has_value());
}

/**
 * Given the ship of a player whose client showed tick 95, and a missile it
 * fired
 * When the arbitration between players gives its views at tick 100
 * Then the missile is tested five ticks behind, where its shooter saw it
 */
TEST_F(HitArbitrationFixture,
       TheShooterArbitrationTestsTheMissileWhereItsShooterSaw) {
  giveViewsOf(FavorTheShooter{});

  EXPECT_EQ(missileDelay().value_or(0), FIVE_TICKS_BEHIND);
}

/**
 * Given a missile whose player's client showed tick 70, thirty ticks before
 * the present
 * When the arbitration between players gives its views at tick 100
 * Then the missile is tested 12 ticks behind, 200 ms: a shooter with a long
 * ping cannot hit what another player left long ago
 */
TEST_F(HitArbitrationFixture, TheShooterArbitrationLooksBack200Ms) {
  seeThirtyTicksBehind();

  giveViewsOf(FavorTheShooter{});

  EXPECT_EQ(missileDelay().value_or(0), SHOOTER_REWIND_IN_TICKS);
}
