#include <gtest/gtest.h>
#include "Position.hpp"
#include "ProjectileStartPosition.hpp"
#include "ProjectileStartPositionTestConstants.hpp"

using rtype::game::Position;
using rtype::game::projectileStartPosition;
using rtype::game::testing::BYDO_MISSILE_START_X;
using rtype::game::testing::ENEMY_MISSILE_BOX;
using rtype::game::testing::PLAYER_MISSILE_BOX;
using rtype::game::testing::SHIP_MISSILE_START_X;
using rtype::game::testing::SOME_BYDO;
using rtype::game::testing::SOME_SHIP;

/**
 * Given a ship of a player
 * When the start of its missile is asked for
 * Then the missile starts with its back against the front of the ship
 */
TEST(ProjectileStartPosition, AShipFiresFromItsNose) {
  const Position missile =
      projectileStartPosition(SOME_SHIP, PLAYER_MISSILE_BOX);

  EXPECT_FLOAT_EQ(missile.x, SHIP_MISSILE_START_X);
}

/**
 * Given a ship of a player
 * When the start of its missile is asked for
 * Then the missile starts on the row of the ship
 */
TEST(ProjectileStartPosition, AShipFiresOnItsRow) {
  const Position missile =
      projectileStartPosition(SOME_SHIP, PLAYER_MISSILE_BOX);

  EXPECT_FLOAT_EQ(missile.y, SOME_SHIP.center.y);
}

/**
 * Given an enemy
 * When the start of its missile is asked for
 * Then the missile starts with its right side against the left side of the
 * enemy, the front of an enemy, which flies left
 */
TEST(ProjectileStartPosition, AnEnemyFiresFromItsLeftSide) {
  const Position missile =
      projectileStartPosition(SOME_BYDO, ENEMY_MISSILE_BOX);

  EXPECT_FLOAT_EQ(missile.x, BYDO_MISSILE_START_X);
}

/**
 * Given an enemy
 * When the start of its missile is asked for
 * Then the missile starts on the row of the enemy
 */
TEST(ProjectileStartPosition, AnEnemyFiresOnItsRow) {
  const Position missile =
      projectileStartPosition(SOME_BYDO, ENEMY_MISSILE_BOX);

  EXPECT_FLOAT_EQ(missile.y, SOME_BYDO.center.y);
}
