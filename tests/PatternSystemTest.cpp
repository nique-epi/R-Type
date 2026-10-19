#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "MovementPattern.hpp"
#include "PatternSystem.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::MovementPattern;
using rtype::game::PatternShape;
using rtype::game::PatternSystem;
using rtype::game::Position;

namespace {

constexpr float ROW_Y = 300.0F;
constexpr float SOME_X = 800.0F;
constexpr float WAVE_BOTTOM_Y = 348.0F;
constexpr float HEIGHT_TOLERANCE_UNITS = 1e-3F;
constexpr Duration QUARTER_PERIOD = std::chrono::milliseconds(500);
constexpr Duration HALF_PERIOD = std::chrono::seconds(1);
constexpr std::size_t TICKS_IN_QUARTER_PERIOD = 30;

Entity addPatterned(EntityRegistry& entities, ComponentRegistry& components,
                    PatternShape shape) {
  const Entity entity = entities.create();
  components.add(entity, Position{.x = SOME_X, .y = ROW_Y});
  components.add(
      entity,
      MovementPattern{.shape = shape, .originY = ROW_Y, .ageSeconds = 0.0F});
  return entity;
}

}  // namespace

/**
 * Given an entity on a Straight pattern, on the row at height 300
 * When one second of simulation time passes
 * Then it is still at height 300
 */
TEST(PatternSystem, StraightKeepsItsRow) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity =
      addPatterned(entities, components, PatternShape::Straight);
  PatternSystem pattern;

  pattern.update(components, HALF_PERIOD);

  EXPECT_FLOAT_EQ(components.get<Position>(entity)->y, ROW_Y);
}

/**
 * Given an entity on a Wave pattern, on the row at height 300
 * When a quarter of the 2 second period has passed
 * Then it is 48 units below its row, at the lowest point of the wave (y grows
 * downwards)
 */
TEST(PatternSystem, WaveReachesItsLowestPointAtAQuarterPeriod) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = addPatterned(entities, components, PatternShape::Wave);
  PatternSystem pattern;

  pattern.update(components, QUARTER_PERIOD);

  EXPECT_NEAR(components.get<Position>(entity)->y, WAVE_BOTTOM_Y,
              HEIGHT_TOLERANCE_UNITS);
}

/**
 * Given an entity on a Wave pattern
 * When half of the period has passed
 * Then it is back on its row
 */
TEST(PatternSystem, WaveComesBackToItsRowAtHalfPeriod) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity = addPatterned(entities, components, PatternShape::Wave);
  PatternSystem pattern;

  pattern.update(components, HALF_PERIOD);

  EXPECT_NEAR(components.get<Position>(entity)->y, ROW_Y,
              HEIGHT_TOLERANCE_UNITS);
}

/**
 * Given two worlds, each with an entity on the same Wave pattern
 * When one gets half a second in one step and the other the same half second
 * in 30 simulation ticks
 * Then they are at the same height
 */
TEST(PatternSystem, WavePathDoesNotDependOnTheStep) {
  EntityRegistry oneStepEntities;
  ComponentRegistry oneStepComponents(oneStepEntities);
  const Entity oneStep =
      addPatterned(oneStepEntities, oneStepComponents, PatternShape::Wave);
  EntityRegistry manyStepsEntities;
  ComponentRegistry manyStepsComponents(manyStepsEntities);
  const Entity manySteps =
      addPatterned(manyStepsEntities, manyStepsComponents, PatternShape::Wave);
  PatternSystem pattern;

  pattern.update(oneStepComponents, QUARTER_PERIOD);
  for (std::size_t tick = 0; tick < TICKS_IN_QUARTER_PERIOD; ++tick) {
    pattern.update(manyStepsComponents, SIMULATION_TICK_DURATION);
  }

  EXPECT_NEAR(manyStepsComponents.get<Position>(manySteps)->y,
              oneStepComponents.get<Position>(oneStep)->y,
              HEIGHT_TOLERANCE_UNITS);
}
