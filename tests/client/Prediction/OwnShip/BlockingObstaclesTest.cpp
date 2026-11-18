#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "BlockingObstacles.hpp"
#include "BlockingObstaclesTestConstants.hpp"
#include "Bounds.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "Obstacle.hpp"
#include "ObstacleContact.hpp"
#include "Position.hpp"

using rtype::client::collectBlockingObstacles;
using rtype::client::scrollLeadOf;
using rtype::client::testing::DRAWN_WALL_BOX;
using rtype::client::testing::LEAD_OF_SIX_TICKS;
using rtype::client::testing::SCROLL_SPEED_OF_A_LEVEL;
using rtype::client::testing::SERVER_TICK_BEHIND;
using rtype::client::testing::SERVER_TICK_SIX_AHEAD;
using rtype::client::testing::TICK_DRAWN;
using rtype::client::testing::WALL_CENTER_AHEAD_X;
using rtype::client::testing::WHERE_THE_WALL_IS_DRAWN;
using rtype::engine::Bounds;
using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::Obstacle;
using rtype::game::ObstacleContact;

/**
 * Given a level scrolling at 120 units a second, drawn at tick 100 while the
 * server is at tick 106
 * When the lead of the walls is computed
 * Then it is 12 units: a tenth of a second of scroll
 */
TEST(BlockingObstacles, ALevelScrollingAt120LeadsTwoUnitsATick) {
  EXPECT_FLOAT_EQ(
      scrollLeadOf(SCROLL_SPEED_OF_A_LEVEL, SERVER_TICK_SIX_AHEAD, TICK_DRAWN),
      LEAD_OF_SIX_TICKS);
}

/**
 * Given a level scrolling at 120 units a second, and no clock of the server
 * When the lead of the walls is computed
 * Then it is 0
 */
TEST(BlockingObstacles, NoLeadWithoutTheClockOfTheServer) {
  EXPECT_FLOAT_EQ(
      scrollLeadOf(SCROLL_SPEED_OF_A_LEVEL, std::nullopt, TICK_DRAWN), 0.0F);
}

/**
 * Given a level scrolling at 120 units a second, and a server that seems
 * behind the tick drawn
 * When the lead of the walls is computed
 * Then it is 0: a wall is never moved back
 */
TEST(BlockingObstacles, NoLeadWhenTheServerSeemsBehind) {
  EXPECT_FLOAT_EQ(
      scrollLeadOf(SCROLL_SPEED_OF_A_LEVEL, SERVER_TICK_BEHIND, TICK_DRAWN),
      0.0F);
}

/**
 * Given a blocking wall and a fatal wall drawn at x 600
 * When the blocking walls are collected with a lead of 12 units
 * Then only the blocking wall is collected, its box centered at x 588
 */
TEST(BlockingObstacles, CollectsTheBlockingWallsMovedAhead) {
  EntityRegistry entities;
  ComponentRegistry components{entities};
  for (const ObstacleContact contact :
       {ObstacleContact::Blocking, ObstacleContact::Fatal}) {
    const Entity wall = entities.create();
    components.add(wall, WHERE_THE_WALL_IS_DRAWN);
    components.add(wall, DRAWN_WALL_BOX);
    components.add(wall, Obstacle{.contact = contact});
  }
  std::vector<Bounds> boxes;

  collectBlockingObstacles(components, LEAD_OF_SIX_TICKS, boxes);

  ASSERT_EQ(boxes.size(), 1U);
  EXPECT_FLOAT_EQ(boxes[0].centerX, WALL_CENTER_AHEAD_X);
  EXPECT_FLOAT_EQ(boxes[0].centerY, WHERE_THE_WALL_IS_DRAWN.y);
  EXPECT_FLOAT_EQ(boxes[0].width, DRAWN_WALL_BOX.width);
}
