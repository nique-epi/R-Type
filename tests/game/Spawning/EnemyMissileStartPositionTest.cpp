#include <gtest/gtest.h>
#include "EnemyMissileStartPosition.hpp"
#include "EnemyMissileStartPositionTestConstants.hpp"
#include "Position.hpp"

using rtype::game::enemyMissileStartPosition;
using rtype::game::Position;
using rtype::game::testing::ENEMY_MISSILE_START_X;
using rtype::game::testing::SOME_BYDO;

/**
 * Given a Bydo
 * When the start of its missile is asked for
 * Then the missile starts with its right side against the left side of the
 * Bydo, the front of a Bydo, which flies left
 */
TEST(EnemyMissileStartPosition, StartsAgainstTheFrontOfTheBydo) {
  const Position missile = enemyMissileStartPosition(SOME_BYDO);

  EXPECT_FLOAT_EQ(missile.x, ENEMY_MISSILE_START_X);
}

/**
 * Given a Bydo
 * When the start of its missile is asked for
 * Then the missile starts on the row of the Bydo
 */
TEST(EnemyMissileStartPosition, StartsOnTheRowOfTheBydo) {
  const Position missile = enemyMissileStartPosition(SOME_BYDO);

  EXPECT_FLOAT_EQ(missile.y, SOME_BYDO.y);
}
