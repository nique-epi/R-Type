#include <gtest/gtest.h>
#include "MissileStartPosition.hpp"
#include "Position.hpp"

namespace {

/** @brief A ship somewhere in the play area. */
constexpr rtype::game::Position SOME_SHIP{.x = 500.0F, .y = 300.0F};

/**
 * @brief Where a missile of SOME_SHIP starts: the front of the ship is half of
 * its 128 units past its center, at 564, and the center of the missile is half
 * of its 64 units further, at 596.
 */
constexpr float MISSILE_START_X = 596.0F;

}  // namespace

/**
 * Given a ship
 * When the start of its missile is asked for
 * Then the missile starts with its back against the front of the ship
 */
TEST(MissileStartPosition, StartsAgainstTheFrontOfTheShip) {
  const rtype::game::Position missile =
      rtype::game::missileStartPosition(SOME_SHIP);

  EXPECT_FLOAT_EQ(missile.x, MISSILE_START_X);
}

/**
 * Given a ship
 * When the start of its missile is asked for
 * Then the missile starts on the row of the ship
 */
TEST(MissileStartPosition, StartsOnTheRowOfTheShip) {
  const rtype::game::Position missile =
      rtype::game::missileStartPosition(SOME_SHIP);

  EXPECT_FLOAT_EQ(missile.y, SOME_SHIP.y);
}
