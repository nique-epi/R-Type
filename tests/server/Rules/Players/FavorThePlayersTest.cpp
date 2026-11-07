#include <gtest/gtest.h>
#include <cstddef>
#include "FavorThePlayers.hpp"
#include "HitArbitrationFixture.hpp"
#include "ShooterViewsTestConstants.hpp"

using rtype::server::FavorThePlayers;
using rtype::server::testing::FIVE_TICKS_BEHIND;
using rtype::server::testing::HitArbitrationFixture;

/**
 * Given the ship of a player whose client showed tick 95, and a missile it
 * fired
 * When the co-op arbitration gives its views at tick 100
 * Then the ship is tested five ticks behind, where its player saw the
 * enemies: what it saw miss it does not hit it
 */
TEST_F(HitArbitrationFixture, TheCoopArbitrationTestsTheShipWhereItsPlayerSaw) {
  giveViewsOf(FavorThePlayers{});

  EXPECT_EQ(shipDelay().value_or(0), FIVE_TICKS_BEHIND);
}

/**
 * Given the ship of a player whose client showed tick 95, and a missile it
 * fired
 * When the co-op arbitration gives its views at tick 100
 * Then the missile is tested five ticks behind too
 */
TEST_F(HitArbitrationFixture,
       TheCoopArbitrationTestsTheMissileWhereItsPlayerSaw) {
  giveViewsOf(FavorThePlayers{});

  EXPECT_EQ(missileDelay().value_or(0), FIVE_TICKS_BEHIND);
}
