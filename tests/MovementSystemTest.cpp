#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include "ComponentRegistry.hpp"
#include "Direction.hpp"
#include "DirectionConstants.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "Intent.hpp"
#include "MovementSystem.hpp"
#include "MovementSystemFixtures.hpp"
#include "MovementSystemTestConstants.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ShipMotion.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"
#include "Velocity.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::engine::testing::ONE_SECOND;
using rtype::game::Direction;
using rtype::game::FULL_STEERING;
using rtype::game::Intent;
using rtype::game::MovementSystem;
using rtype::game::PLAYFIELD_WIDTH;
using rtype::game::Position;
using rtype::game::ShipMotion;
using rtype::game::Velocity;
using rtype::game::testing::HALF_SECOND;
using rtype::game::testing::HALF_SECOND_ARRIVAL_X;
using rtype::game::testing::HALF_SECOND_ARRIVAL_Y;
using rtype::game::testing::POSITION_TOLERANCE_UNITS;
using rtype::game::testing::runOneSecondAt;
using rtype::game::testing::SPEED_X;
using rtype::game::testing::SPEED_Y;
using rtype::game::testing::START_X;
using rtype::game::testing::START_Y;

/**
 * Given an entity at (100, 200) moving at (120, -60) units per second
 * When half a second of movement is applied
 * Then it is at (160, 170)
 */
TEST(MovementSystem, MovesByVelocityTimesElapsedTime) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity ship = entities.create();
  components.add(ship, Position{.x = START_X, .y = START_Y});
  components.add(ship, Velocity{.x = SPEED_X, .y = SPEED_Y});
  MovementSystem movement;

  movement.update(components, HALF_SECOND);

  const Position* position = components.get<Position>(ship);
  ASSERT_NE(position, nullptr);
  EXPECT_FLOAT_EQ(position->x, HALF_SECOND_ARRIVAL_X);
  EXPECT_FLOAT_EQ(position->y, HALF_SECOND_ARRIVAL_Y);
}

/**
 * Given a ship at the left edge moving right at 120 units per second
 * When one second is rendered at 30, 60 and 144 frames per second, the
 * movement running once per simulation tick
 * Then the ship moved 120 units at every frame rate
 */
TEST(MovementSystem,
     SameDistanceAtThirtySixtyAndOneHundredFortyFourFramesPerSecond) {
  for (const std::int64_t framesPerSecond : {30, 60, 144}) {
    EntityRegistry entities;
    ComponentRegistry components(entities);
    const Entity ship = entities.create();
    components.add(ship, Position{.x = 0.0F, .y = START_Y});
    components.add(ship, Velocity{.x = SPEED_X, .y = 0.0F});

    runOneSecondAt(framesPerSecond, components);

    const Position* position = components.get<Position>(ship);
    ASSERT_NE(position, nullptr);
    EXPECT_NEAR(position->x, SPEED_X, POSITION_TOLERANCE_UNITS)
        << framesPerSecond << " frames per second";
  }
}

/**
 * Given a ship on the left edge moving right at one playfield width per second
 * When one second of movement is applied
 * Then it is on the right edge
 */
TEST(MovementSystem, ShipCrossesThePlayfieldInOneSecondAtOneWidthPerSecond) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity ship = entities.create();
  components.add(ship, Position{.x = 0.0F, .y = START_Y});
  components.add(ship, Velocity{.x = PLAYFIELD_WIDTH, .y = 0.0F});
  MovementSystem movement;

  movement.update(components, ONE_SECOND);

  const Position* position = components.get<Position>(ship);
  ASSERT_NE(position, nullptr);
  EXPECT_FLOAT_EQ(position->x, PLAYFIELD_WIDTH);
}

/**
 * Given an entity with a Position and no Velocity
 * When half a second of movement is applied
 * Then its Position is unchanged
 */
TEST(MovementSystem, EntityWithoutVelocityStaysInPlace) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity wall = entities.create();
  components.add(wall, Position{.x = START_X, .y = START_Y});
  MovementSystem movement;

  movement.update(components, HALF_SECOND);

  const Position* position = components.get<Position>(wall);
  ASSERT_NE(position, nullptr);
  EXPECT_FLOAT_EQ(position->x, START_X);
  EXPECT_FLOAT_EQ(position->y, START_Y);
}

/**
 * Given an entity at (100, 200) moving at (120, -60) units per second
 * When movement is applied for zero time
 * Then it is still at (100, 200)
 */
TEST(MovementSystem, ZeroElapsedTimeMovesNothing) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity ship = entities.create();
  components.add(ship, Position{.x = START_X, .y = START_Y});
  components.add(ship, Velocity{.x = SPEED_X, .y = SPEED_Y});
  MovementSystem movement;

  movement.update(components, Duration::zero());

  const Position* position = components.get<Position>(ship);
  ASSERT_NE(position, nullptr);
  EXPECT_FLOAT_EQ(position->x, START_X);
  EXPECT_FLOAT_EQ(position->y, START_Y);
}

/**
 * Given a ship at (100, 200) whose Intent holds right
 * When one tick of movement is applied
 * Then it moved right and not vertically, and it drifts right
 */
TEST(MovementSystem, ShipMovesByTheDirectionsOfItsIntent) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity ship = entities.create();
  components.add(ship, Position{.x = START_X, .y = START_Y});
  components.add(ship, ShipMotion{});
  components.add(ship, Intent{.direction = Direction{.x = FULL_STEERING}});
  MovementSystem movement;

  movement.update(components, SIMULATION_TICK_DURATION);

  const Position* position = components.get<Position>(ship);
  const ShipMotion* motion = components.get<ShipMotion>(ship);
  ASSERT_NE(position, nullptr);
  ASSERT_NE(motion, nullptr);
  EXPECT_GT(position->x, START_X);
  EXPECT_FLOAT_EQ(position->y, START_Y);
  EXPECT_GT(motion->velocityX, 0.0F);
}

/**
 * Given a ship at rest whose Intent holds nothing
 * When one tick of movement is applied
 * Then it stays in place
 */
TEST(MovementSystem, ShipWhoseIntentHoldsNothingStaysInPlace) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity ship = entities.create();
  components.add(ship, Position{.x = START_X, .y = START_Y});
  components.add(ship, ShipMotion{});
  components.add(ship, Intent{});
  MovementSystem movement;

  movement.update(components, SIMULATION_TICK_DURATION);

  const Position* position = components.get<Position>(ship);
  ASSERT_NE(position, nullptr);
  EXPECT_FLOAT_EQ(position->x, START_X);
  EXPECT_FLOAT_EQ(position->y, START_Y);
}

/**
 * Given an entity with a ShipMotion and no Intent
 * When one tick of movement is applied
 * Then it does not move: no controller wants it anywhere
 */
TEST(MovementSystem, BodyWithoutIntentIsNotMovedByIt) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity body = entities.create();
  components.add(body, Position{.x = START_X, .y = START_Y});
  components.add(body, ShipMotion{.velocityX = SPEED_X});
  MovementSystem movement;

  movement.update(components, SIMULATION_TICK_DURATION);

  const Position* position = components.get<Position>(body);
  ASSERT_NE(position, nullptr);
  EXPECT_FLOAT_EQ(position->x, START_X);
}
