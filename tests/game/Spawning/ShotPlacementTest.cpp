#include <gtest/gtest.h>
#include <cmath>
#include "BodyPlacement.hpp"
#include "EntityType.hpp"
#include "ShotPattern.hpp"
#include "ShotPlacement.hpp"
#include "ShotPlacementTestConstants.hpp"
#include "ShotStart.hpp"

using rtype::game::BodyPlacement;
using rtype::game::EntityType;
using rtype::game::ShotPattern;
using rtype::game::shotPlacement;
using rtype::game::ShotStart;
using rtype::game::testing::FAN_DISTANCE_AHEAD;
using rtype::game::testing::FAN_OF_THREE;
using rtype::game::testing::FAN_SIDE_AHEAD;
using rtype::game::testing::FAN_SIDE_ASIDE;
using rtype::game::testing::FIRST_OF_THE_FAN;
using rtype::game::testing::HALF_THE_ROWS;
using rtype::game::testing::LAST_OF_THE_FAN;
using rtype::game::testing::LOWER_ROW;
using rtype::game::testing::MIDDLE_OF_THE_FAN;
using rtype::game::testing::ONE_AHEAD;
using rtype::game::testing::ONLY_SHOT;
using rtype::game::testing::SHOT_START;
using rtype::game::testing::SHOT_TOLERANCE_UNITS;
using rtype::game::testing::TWO_ROWS;
using rtype::game::testing::UPPER_ROW;

/**
 * Given the shot of a gun of one projectile, straight ahead
 * When its projectile is placed
 * Then it appears and steers from the start, as a single missile does
 */
TEST(ShotPlacement, ASingleShotStartsAtTheStart) {
  const BodyPlacement placement = shotPlacement(
      ShotPattern{}, ShotStart{.point = SHOT_START, .side = EntityType::Player},
      ONLY_SHOT);

  EXPECT_FLOAT_EQ(placement.position.x, SHOT_START.x);
  EXPECT_FLOAT_EQ(placement.position.y, SHOT_START.y);
  EXPECT_FLOAT_EQ(placement.origin.x, SHOT_START.x);
  EXPECT_FLOAT_EQ(placement.origin.y, SHOT_START.y);
}

/**
 * Given a fan of three projectiles over 24 degrees, 16 units ahead, fired by
 * a ship of a player
 * When its middle projectile is placed
 * Then it lies 16 units to the right of the start, on its row
 */
TEST(ShotPlacement, TheMiddleOfAFanFliesStraightAhead) {
  const BodyPlacement placement = shotPlacement(
      FAN_OF_THREE, ShotStart{.point = SHOT_START, .side = EntityType::Player},
      MIDDLE_OF_THE_FAN);

  EXPECT_NEAR(placement.position.x, SHOT_START.x + FAN_DISTANCE_AHEAD,
              SHOT_TOLERANCE_UNITS);
  EXPECT_NEAR(placement.position.y, SHOT_START.y, SHOT_TOLERANCE_UNITS);
  EXPECT_FLOAT_EQ(placement.origin.x, SHOT_START.x);
}

/**
 * Given a fan of three projectiles over 24 degrees, 16 units ahead, fired by
 * a ship of a player
 * When its first and last projectiles are placed
 * Then they lie 12 degrees above and below the middle, each on its side
 */
TEST(ShotPlacement, TheSidesOfAFanTurnApart) {
  const ShotStart start{.point = SHOT_START, .side = EntityType::Player};

  const BodyPlacement first =
      shotPlacement(FAN_OF_THREE, start, FIRST_OF_THE_FAN);
  const BodyPlacement last =
      shotPlacement(FAN_OF_THREE, start, LAST_OF_THE_FAN);

  EXPECT_NEAR(first.position.x, SHOT_START.x + FAN_SIDE_AHEAD,
              SHOT_TOLERANCE_UNITS);
  EXPECT_NEAR(last.position.x, SHOT_START.x + FAN_SIDE_AHEAD,
              SHOT_TOLERANCE_UNITS);
  EXPECT_NEAR(std::abs(first.position.y - SHOT_START.y), FAN_SIDE_ASIDE,
              SHOT_TOLERANCE_UNITS);
  EXPECT_NEAR(first.position.y - SHOT_START.y, SHOT_START.y - last.position.y,
              SHOT_TOLERANCE_UNITS);
}

/**
 * Given two rows of projectiles 24 units apart
 * When both are placed
 * Then they lie 12 units above and 12 below the start, each on its own row
 */
TEST(ShotPlacement, RowsAreCentredOnTheStart) {
  const ShotStart start{.point = SHOT_START, .side = EntityType::Player};

  const BodyPlacement upper = shotPlacement(TWO_ROWS, start, UPPER_ROW);
  const BodyPlacement lower = shotPlacement(TWO_ROWS, start, LOWER_ROW);

  EXPECT_FLOAT_EQ(upper.position.y, SHOT_START.y - HALF_THE_ROWS);
  EXPECT_FLOAT_EQ(lower.position.y, SHOT_START.y + HALF_THE_ROWS);
  EXPECT_FLOAT_EQ(upper.position.x, SHOT_START.x);
}

/**
 * Given one projectile laid 16 units ahead, fired by an enemy
 * When it is placed
 * Then it lies 16 units to the left of the start: an enemy fires left
 */
TEST(ShotPlacement, AnEnemyFiresToTheLeft) {
  const BodyPlacement placement = shotPlacement(
      ONE_AHEAD, ShotStart{.point = SHOT_START, .side = EntityType::Enemy},
      ONLY_SHOT);

  EXPECT_NEAR(placement.position.x, SHOT_START.x - FAN_DISTANCE_AHEAD,
              SHOT_TOLERANCE_UNITS);
  EXPECT_NEAR(placement.position.y, SHOT_START.y, SHOT_TOLERANCE_UNITS);
}
