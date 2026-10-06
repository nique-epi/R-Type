#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "FixedTimestep.hpp"
#include "MovementSystem.hpp"
#include "Position.hpp"
#include "SimulatedClock.hpp"
#include "SystemScheduler.hpp"
#include "TimeConstants.hpp"
#include "Velocity.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::engine::FixedTimestep;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::engine::SystemScheduler;
using rtype::game::MovementSystem;
using rtype::game::Position;
using rtype::game::Velocity;

namespace {

constexpr float START_X = 100.0F;
constexpr float START_Y = 200.0F;
constexpr float SPEED_X = 120.0F;
constexpr float SPEED_Y = -60.0F;
constexpr Duration HALF_SECOND = std::chrono::milliseconds(500);
constexpr float HALF_SECOND_ARRIVAL_X = 160.0F;
constexpr float HALF_SECOND_ARRIVAL_Y = 170.0F;
constexpr std::int64_t NANOSECONDS_PER_SECOND = 1'000'000'000;
constexpr float POSITION_TOLERANCE_UNITS = 1e-4F;

/**
 * @brief Renders one second at the given frame rate, running the systems once
 * per tick returned by FixedTimestep. Frame lengths are whole nanoseconds that
 * add up to exactly one second.
 */
void runOneSecondAt(std::int64_t framesPerSecond,
                    ComponentRegistry& components) {
  SimulatedClock clock;
  FixedTimestep timestep(clock, SIMULATION_TICK_DURATION);
  SystemScheduler systems;
  systems.add(std::make_unique<MovementSystem>());

  for (std::int64_t frame = 0; frame < framesPerSecond; ++frame) {
    const std::int64_t start = frame * NANOSECONDS_PER_SECOND / framesPerSecond;
    const std::int64_t end =
        (frame + 1) * NANOSECONDS_PER_SECOND / framesPerSecond;
    clock.advance(Duration(end - start));
    const std::size_t ticks = timestep.consumeTicks();
    for (std::size_t tick = 0; tick < ticks; ++tick) {
      systems.run(components, SIMULATION_TICK_DURATION);
    }
  }
}

}  // namespace

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
