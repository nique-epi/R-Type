#pragma once

#include <cstddef>
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"

namespace rtype::engine::testing {

/**
 * @returns How many ticks of SIMULATION_TICK_DURATION run until @p duration of
 * simulation time has passed, the tick that reaches it included. A tick lasts
 * a whole number of nanoseconds, a little under a sixtieth of a second, so 2
 * seconds are reached on the 121st tick and 4 seconds on the 241st.
 */
constexpr std::size_t ticksToReach(Duration duration) {
  return static_cast<std::size_t>(
      (duration + SIMULATION_TICK_DURATION - ONE_NANOSECOND) /
      SIMULATION_TICK_DURATION);
}

}  // namespace rtype::engine::testing
