#include <gtest/gtest.h>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "Position.hpp"
#include "ScrollMotion.hpp"
#include "ScrollingBodies.hpp"
#include "ScrollingBodiesTestConstants.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::carryAlongTheScroll;
using rtype::game::Position;
using rtype::game::ScrollMotion;

/**
 * Given a body that follows the scroll and a body that does not, both at
 * x 500
 * When the level scrolls 2 units
 * Then the first is at x 498, on its row, and the second has not moved
 */
TEST(ScrollingBodies, CarriesOnlyTheBodiesThatFollowTheScroll) {
  EntityRegistry entities;
  ComponentRegistry components{entities};
  const Entity wall = entities.create();
  components.add(wall, Position{.x = BODY_START_X, .y = BODY_ROW_Y});
  components.add(wall, ScrollMotion{});
  const Entity bydo = entities.create();
  components.add(bydo, Position{.x = BODY_START_X, .y = BODY_ROW_Y});

  carryAlongTheScroll(components, ONE_TICK_OF_SCROLL);

  EXPECT_FLOAT_EQ(components.get<Position>(wall)->x, CARRIED_ONE_TICK_X);
  EXPECT_FLOAT_EQ(components.get<Position>(wall)->y, BODY_ROW_Y);
  EXPECT_FLOAT_EQ(components.get<Position>(bydo)->x, BODY_START_X);
}

/**
 * Given a body that follows the scroll at x 500
 * When the level does not scroll
 * Then the body stays at x 500
 */
TEST(ScrollingBodies, APausedScrollCarriesNothing) {
  EntityRegistry entities;
  ComponentRegistry components{entities};
  const Entity wall = entities.create();
  components.add(wall, Position{.x = BODY_START_X, .y = BODY_ROW_Y});
  components.add(wall, ScrollMotion{});

  carryAlongTheScroll(components, NO_SCROLL);

  EXPECT_FLOAT_EQ(components.get<Position>(wall)->x, BODY_START_X);
}
