#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include "EngineException.hpp"
#include "FixedTimestep.hpp"
#include "FixedTimestepTestConstants.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Duration;
using rtype::engine::FixedTimestep;
using rtype::engine::MAXIMUM_TICKS_PER_ADVANCE;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::engine::SIMULATION_TICKS_PER_SECOND;

namespace {

struct alignas(RESULT_ALIGNMENT) SimulationResult {
  std::size_t ticks = 0;
  double position = 0.0;
};

/**
 * @brief Renders one simulated second at the given frame rate. Frame lengths
 * are whole nanoseconds that add up to exactly one second.
 */
SimulationResult simulateOneSecond(std::int64_t framesPerSecond) {
  SimulatedClock clock;
  FixedTimestep timestep(clock, SIMULATION_TICK_DURATION);
  const double tickSeconds =
      std::chrono::duration<double>(SIMULATION_TICK_DURATION).count();
  SimulationResult result;

  for (std::int64_t frame = 0; frame < framesPerSecond; ++frame) {
    const std::int64_t start = frame * NANOSECONDS_PER_SECOND / framesPerSecond;
    const std::int64_t end =
        (frame + 1) * NANOSECONDS_PER_SECOND / framesPerSecond;
    clock.advance(Duration(end - start));
    const std::size_t ticks = timestep.consumeTicks();
    for (std::size_t tick = 0; tick < ticks; ++tick) {
      result.position += SPEED_UNITS_PER_SECOND * tickSeconds;
    }
    result.ticks += ticks;
  }
  return result;
}

}  // namespace

/**
 * Given a ship moving at 120 units per second
 * When one second is rendered at 30, 60 and 144 frames per second
 * Then each run simulates 60 ticks and the ship moved 120 units
 */
TEST(FixedTimestep,
     SameResultAtThirtySixtyAndOneHundredFortyFourFramesPerSecond) {
  for (const std::int64_t framesPerSecond : {30, 60, 144}) {
    const SimulationResult result = simulateOneSecond(framesPerSecond);

    EXPECT_EQ(result.ticks, SIMULATION_TICKS_PER_SECOND)
        << framesPerSecond << " frames per second";
    EXPECT_NEAR(result.position, SPEED_UNITS_PER_SECOND,
                POSITION_TOLERANCE_UNITS)
        << framesPerSecond << " frames per second";
  }
}

/**
 * Given a fresh timestep
 * When less than one tick has elapsed
 * Then no tick is due
 */
TEST(FixedTimestep, NoTickBeforeOneTickHasElapsed) {
  SimulatedClock clock;
  FixedTimestep timestep(clock, SIMULATION_TICK_DURATION);

  clock.advance(SIMULATION_TICK_DURATION - Duration(1));

  EXPECT_EQ(timestep.consumeTicks(), 0U);
}

/**
 * Given a fresh timestep
 * When exactly one tick has elapsed
 * Then one tick is due
 */
TEST(FixedTimestep, OneTickWhenExactlyOneTickHasElapsed) {
  SimulatedClock clock;
  FixedTimestep timestep(clock, SIMULATION_TICK_DURATION);

  clock.advance(SIMULATION_TICK_DURATION);

  EXPECT_EQ(timestep.consumeTicks(), 1U);
}

/**
 * Given a timestep that was polled after half a tick
 * When another half tick elapses
 * Then the two halves add up to one tick
 */
TEST(FixedTimestep, RemainderIsKeptBetweenCalls) {
  SimulatedClock clock;
  FixedTimestep timestep(clock, SHORT_TICK_DURATION);

  clock.advance(HALF_SHORT_TICK);
  const std::size_t first = timestep.consumeTicks();
  clock.advance(HALF_SHORT_TICK);
  const std::size_t second = timestep.consumeTicks();

  EXPECT_EQ(first, 0U);
  EXPECT_EQ(second, 1U);
}

/**
 * Given a timestep
 * When one second passes without being polled
 * Then only the maximum number of ticks is due, and nothing is owed afterwards
 */
TEST(FixedTimestep, LongStallIsCappedAndTheExcessDropped) {
  SimulatedClock clock;
  FixedTimestep timestep(clock, SIMULATION_TICK_DURATION);

  clock.advance(std::chrono::seconds(1));
  const std::size_t afterStall = timestep.consumeTicks();
  const std::size_t immediately = timestep.consumeTicks();

  EXPECT_EQ(afterStall, MAXIMUM_TICKS_PER_ADVANCE);
  EXPECT_EQ(immediately, 0U);
}

/**
 * Given a tick duration of zero
 * When a timestep is built
 * Then it is rejected
 */
TEST(FixedTimestep, ZeroTickDurationIsRejected) {
  const SimulatedClock clock;

  EXPECT_THROW(FixedTimestep(clock, Duration::zero()),
               rtype::engine::InvalidTickDurationException);
}
