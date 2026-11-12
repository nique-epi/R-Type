#include <gtest/gtest.h>
#include <array>
#include <span>
#include "Bounds.hpp"
#include "ObstacleBlocking.hpp"
#include "ObstacleBlockingTestConstants.hpp"
#include "ShipControl.hpp"
#include "ShipMotion.hpp"

using rtype::engine::Bounds;
using rtype::game::blockedByObstacles;
using rtype::game::MovingShip;
using rtype::game::ShipMotion;
using rtype::game::testing::CLEAR_ABOVE;
using rtype::game::testing::CLEAR_ON_THE_LEFT;
using rtype::game::testing::FULL_SPEED_RIGHT;
using rtype::game::testing::INTO_THE_LEFT_SIDE;
using rtype::game::testing::INTO_THE_TOP;
using rtype::game::testing::PUSHED_BY_THE_WALL_X;
using rtype::game::testing::SOME_SPEED_DOWN;
using rtype::game::testing::SOME_SPEED_UP_THE_SCREEN;
using rtype::game::testing::TOUCHING_THE_LEFT_SIDE;
using rtype::game::testing::TOUCHING_THE_LEFT_SIDE_X;
using rtype::game::testing::TOUCHING_THE_TOP_Y;
using rtype::game::testing::WALL;
using rtype::game::testing::WALL_MIDDLE_X;
using rtype::game::testing::WALL_MOVED_ONTO_THE_SHIP;
using rtype::game::testing::WALL_ROW_Y;

/**
 * Given a ship clear of a wall at x 396, then 6 units into its left side at
 * x 410, at full speed right
 * When it is blocked by the wall
 * Then it touches its left side, at x 404, and has lost its speed across
 */
TEST(ObstacleBlocking, AShipFlyingIntoAWallStopsAtItsLeftSide) {
  const std::array walls{WALL};

  const MovingShip blocked = blockedByObstacles(
      MovingShip{.position = CLEAR_ON_THE_LEFT, .motion = {}},
      MovingShip{.position = INTO_THE_LEFT_SIDE,
                 .motion = ShipMotion{.velocityX = FULL_SPEED_RIGHT}},
      walls);

  EXPECT_FLOAT_EQ(blocked.position.x, TOUCHING_THE_LEFT_SIDE_X);
  EXPECT_FLOAT_EQ(blocked.position.y, WALL_ROW_Y);
  EXPECT_FLOAT_EQ(blocked.motion.velocityX, 0.0F);
}

/**
 * Given a ship flying right and down into the left side of a wall
 * When it is blocked by the wall
 * Then it keeps its speed down: only the axis it was stopped along is lost
 */
TEST(ObstacleBlocking, AShipStoppedAcrossKeepsItsSpeedDown) {
  const std::array walls{WALL};

  const MovingShip blocked = blockedByObstacles(
      MovingShip{.position = CLEAR_ON_THE_LEFT, .motion = {}},
      MovingShip{.position = INTO_THE_LEFT_SIDE,
                 .motion = ShipMotion{.velocityX = FULL_SPEED_RIGHT,
                                      .velocityY = SOME_SPEED_DOWN}},
      walls);

  EXPECT_FLOAT_EQ(blocked.motion.velocityY, SOME_SPEED_DOWN);
}

/**
 * Given a ship clear above a wall at y 230, then 10 units into its top at
 * y 250
 * When it is blocked by the wall
 * Then it rests on its top, at y 240, and has lost its speed down
 */
TEST(ObstacleBlocking, AShipComingDownOnAWallStopsOnItsTop) {
  const std::array walls{WALL};

  const MovingShip blocked = blockedByObstacles(
      MovingShip{.position = CLEAR_ABOVE, .motion = {}},
      MovingShip{.position = INTO_THE_TOP,
                 .motion = ShipMotion{.velocityY = SOME_SPEED_UP_THE_SCREEN}},
      walls);

  EXPECT_FLOAT_EQ(blocked.position.x, WALL_MIDDLE_X);
  EXPECT_FLOAT_EQ(blocked.position.y, TOUCHING_THE_TOP_Y);
  EXPECT_FLOAT_EQ(blocked.motion.velocityY, 0.0F);
}

/**
 * Given a ship that touched the left side of a wall, and the wall moved 2
 * units onto it by the scroll
 * When it is blocked by the wall
 * Then it is pushed 2 units left, the least deep way, to x 402
 */
TEST(ObstacleBlocking, AWallMovingOntoAShipPushesItTheLeastDeepWay) {
  const std::array walls{WALL_MOVED_ONTO_THE_SHIP};
  const MovingShip ship{.position = TOUCHING_THE_LEFT_SIDE, .motion = {}};

  const MovingShip blocked = blockedByObstacles(ship, ship, walls);

  EXPECT_FLOAT_EQ(blocked.position.x, PUSHED_BY_THE_WALL_X);
  EXPECT_FLOAT_EQ(blocked.position.y, WALL_ROW_Y);
}

/**
 * Given a ship whose right side lies on the left side of a wall, at x 404,
 * flying right
 * When it is blocked by the wall
 * Then it is not moved and keeps its speed: touching along an edge is no
 * overlap
 */
TEST(ObstacleBlocking, AShipTouchingAWallAlongAnEdgeIsNotMoved) {
  const std::array walls{WALL};
  const MovingShip ship{.position = TOUCHING_THE_LEFT_SIDE,
                        .motion = ShipMotion{.velocityX = FULL_SPEED_RIGHT}};

  const MovingShip blocked = blockedByObstacles(ship, ship, walls);

  EXPECT_FLOAT_EQ(blocked.position.x, TOUCHING_THE_LEFT_SIDE_X);
  EXPECT_FLOAT_EQ(blocked.motion.velocityX, FULL_SPEED_RIGHT);
}

/**
 * Given a ship 6 units into the place of a wall, and no wall
 * When it is blocked by no obstacle
 * Then it is where it went
 */
TEST(ObstacleBlocking, NoObstacleBlocksNothing) {
  const MovingShip blocked = blockedByObstacles(
      MovingShip{.position = CLEAR_ON_THE_LEFT, .motion = {}},
      MovingShip{.position = INTO_THE_LEFT_SIDE, .motion = {}},
      std::span<const Bounds>{});

  EXPECT_FLOAT_EQ(blocked.position.x, INTO_THE_LEFT_SIDE.x);
}
