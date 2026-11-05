#include "MovementSystemFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include "ComponentRegistry.hpp"
#include "Direction.hpp"
#include "DirectionConstants.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "FixedTimestep.hpp"
#include "Intent.hpp"
#include "MovementSystem.hpp"
#include "MovementSystemTestConstants.hpp"
#include "Position.hpp"
#include "SimulatedClock.hpp"
#include "SteadyMotion.hpp"
#include "SystemScheduler.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

void runOneSecondAt(std::int64_t framesPerSecond,
                    engine::ComponentRegistry& components) {
  SimulatedClock clock;
  engine::FixedTimestep timestep(clock, engine::SIMULATION_TICK_DURATION);
  engine::SystemScheduler systems;
  systems.add(std::make_unique<MovementSystem>());

  for (std::int64_t frame = 0; frame < framesPerSecond; ++frame) {
    const std::int64_t start = frame * NANOSECONDS_PER_SECOND / framesPerSecond;
    const std::int64_t end =
        (frame + 1) * NANOSECONDS_PER_SECOND / framesPerSecond;
    clock.advance(engine::Duration(end - start));
    const std::size_t ticks = timestep.consumeTicks();
    for (std::size_t tick = 0; tick < ticks; ++tick) {
      systems.run(components, engine::SIMULATION_TICK_DURATION);
    }
  }
}

engine::Entity steadyBodyAtSpeed(engine::EntityRegistry& entities,
                                 engine::ComponentRegistry& components) {
  const engine::Entity body = entities.create();
  components.add(body, Position{.x = START_X, .y = START_Y});
  components.add(body, SteadyMotion{.topSpeed = SPEED_X});
  components.add(body, Intent{.direction = Direction{.x = FULL_STEERING,
                                                     .y = SPEED_Y / SPEED_X}});
  return body;
}

}  // namespace rtype::game::testing
